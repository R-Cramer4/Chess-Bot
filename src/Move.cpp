#include "Board.hpp"
#include "Masks.hpp"
#include <cassert>
#include <iostream>

U64 Board::generateMoves(U64 loc, char piece, Color color, bool top, Move *moves){
    U64 mask = 0;

    // generate pseudo legal moves
    switch(piece){
        case 'n':
            mask = getKnightMove(loc, color);
            break;
        case 'p':
            mask = getPawnMove(loc, color);
            break;
        case 'b':
            mask = getBishopMove(loc, color);
            break;
        case 'r':
            mask = getRookMove(loc, color);
            break;
        case 'q':
            mask = getQueenMove(loc, color);
            break;
        case 'k':
            mask = getKingMove(loc, color);
            break;
    }
    // makes sure not going into own pieces
    if(color == WHITE) mask &= ~whitePieces;
    else mask &= ~blackPieces;

    if(!top) return mask;

    int board = getBoard(piece, color);
    U64 temp = mask;
    int i = 0;
    while(temp != 0){
        U64 move = (temp & -temp);
        temp ^= move; // checks this move

        Move m = movePiece(loc, color, piece, move);
        if(isKingInCheck(color)) mask ^= move; 
        // ^ in check so cant go there
        else if(m.to != 0){
            moves[i] = m; // can go here
            i++;
        }
        unMovePiece();
    }

    // handle king castling
    if(piece == 'k'){
        bool c = isKingInCheck(color);
        if(mask & (loc << 2)){
            if(c || !(mask & (loc << 1))){
                mask ^= (loc << 2);
                // can castle kinside
                // cant castle in check
                // takes out of list
                for(int k = 0; k < i; k++){
                    if(moves[k].to == (loc << 2)) moves[k].from = 0;
                }
            }
        }
        if(mask & (loc >> 2)){
            if(c || !(mask & (loc >> 1))){
                // cant castle due to bing in check or cant move one to the left
                mask ^= (loc >> 2); // cant castle in check
                // takes out of the list
                for(int k = 0; k < i; k++){
                    if(moves[k].to == (loc << 2)) moves[k].from = 0;
                }
            }
        }
    }
    if(piece == 'p' && (mask & rank1 || mask & rank8)){
        // need to add promos
        for(int k = 0; k < i; k++){
            if(moves[k].special < 8 && (moves[k].to & rank1 || moves[k].to & rank8)){
                // this move is the promotion
                // need to modify this move and add 3 more
                moves[k].special += 8; // knight promo or with capture
                for(int j = 1; j < 4; j++){
                    Move m = moves[k];
                    m.special += j; // changes special
                    moves[i] = m; // adds to loc
                    cout << (int)moves[i].special << endl;
                    i++;
                }
            }
        }
    }
    //return mask;
    return i; // i is the amount of moves
}
U64 Board::getRookMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = getLSLoc(loc);

    U64 hori = rank1 << (sq & 56);
    U64 vert = aFile << (sq & 7);

    mask |= getActualRay(loc, hori, color);
    mask |= getActualRay(loc, vert, color);

    return mask;
}
U64 Board::getBishopMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = getLSLoc(loc);

    int diag = 8*(sq & 7) - (sq & 56);
    int nort = -diag & (diag >> 31);
    int sout = diag & (-diag >> 31);
    U64 posDiag = (a1h8Diag >> sout) << nort;

    diag = 56 - 8*(sq & 7) - (sq & 56);
    nort = -diag & (diag >> 31);
    sout = diag & (-diag >> 31);
    U64 antiDiag = (h1a8AntiDiag >> sout) << nort;
    
    mask |= getActualRay(loc, posDiag, color);
    mask |= getActualRay(loc, antiDiag, color);

    return mask;
}
U64 Board::getQueenMove(U64 loc, Color color){
    U64 mask = 0;

    mask |= getRookMove(loc, color);
    mask |= getBishopMove(loc, color);

    return mask;
}
U64 Board::getKnightMove(U64 loc, Color color){
    U64 mask = 0;
    unsigned int sq = getLSLoc(loc);

    // shifts the knight mask
    mask = ((sq > 18) * (knightMask << (sq - 18))) | 
        ((sq <= 18) * (knightMask >> (18 - sq)));

    // masks out overflow bits
    sq = ((sq & 0x7) >> 2);
    mask &= (sq * (~abFile)) | (!sq * (~ghFile));

    return mask;
}
U64 Board::getKingMove(U64 loc, Color color){
    U64 mask = 0;
    unsigned int sq = getLSLoc(loc);

    // shifts the king mask
    mask = ((sq > 9) * (kingMask << (sq - 9))) | 
        ((sq <= 9) * (kingMask >> (9 - sq)));

    // masks out overflow bits
    sq = ((sq & 0x7) >> 2);
    mask &= (sq * (~abFile)) | (!sq * (~ghFile));

    // castling
    if(color == WHITE){
        mask |= (((whitePieces & WKPieces) == 0) && whiteCastleKing)
            * (loc << 2);
        mask |= (((whitePieces & WQPieces) == 0) && whiteCastleQueen)
            * (loc >> 2);
    }else{
        mask |= (((blackPieces & BKPieces) == 0) && blackCastleKing)
            * (loc << 2);
        mask |= (((blackPieces & BQPieces) == 0) && blackCastleQueen)
            * (loc >> 2);
    }

    return mask;
}
U64 Board::getPawnMove(U64 loc, Color color){
    U64 mask = 0;
    U64 attack = 0;
    U64 opponentPieces = 0;
    U64 empty = ~(whitePieces | blackPieces);

    if(color == WHITE){
        opponentPieces = blackPieces;
        mask |= (loc << 8); // normal moves

        mask &= empty;
        mask |= (mask & rank3) << 8; // branchless, generates the move twice forward

        // branchless, if a pawn isnt in in the mask it results in a 0 being shifted
        attack = ((loc << 7) & ~hFile) | ((loc << 9) & ~aFile);
        
        // enpassant, checks if on correct side of the board
        opponentPieces |= (enpassantLoc & rank6);
    }else{
        opponentPieces = whitePieces;
        mask |= (loc >> 8);

        mask &= empty;
        mask |= (mask & rank6) >> 8; // branchless

        attack = ((loc >> 7) & ~aFile) | ((loc >> 9) & ~hFile);

        // enpassant, checks if on correct side of the board
        opponentPieces |= (enpassantLoc & rank3);
    }
    mask &= empty; // cant capture/move forwards if something is there
    attack &= opponentPieces; // only can attack

    return mask | attack;
}

U64 Board::getActualRay(U64 loc, U64 ray, Color color){
    // get the most significant 1 that is intersecting the ray while being less significant than loc
    // get the least significant 1 that is intersecting the ray while being more significant than loc

    int locMin = -1;
    int locMax = -1;
    U64 pieces = whitePieces | blackPieces; // holds all pieces
    U64 colPieces = color == WHITE ? whitePieces : blackPieces;

    pieces ^= loc; // holds all pieces except loc
    pieces &= ray; // holds all pieces intersecting the ray

    int intLoc = getLSLoc(loc);
    U64 pieceSq; // compared to loc
    U64 min = 0;
    U64 max = 0;
    U64 mask = ((U64)1 << intLoc) - 1;
    U64 posMask = pieces & ~(mask); // all bits after loc
    U64 negMask = pieces & mask; // bits below loc

    max = posMask & -posMask; // gets the LSB above the loc

    while(negMask != 0){
        pieceSq = (negMask & -negMask); // isolate LSB
        negMask ^= pieceSq; // remove it
        min = pieceSq;
    }


    // locMin is most significant less than sq
    // locMax is least significant greater than sq
    mask = FullBoard;

    if(min != 0){
        locMin = getLSLoc(min);
        // shifts to the right so everything more significant than locMax is a 0
        ray = ray & (mask << locMin);
    }
    if(max != 0){
        locMax = getLSLoc(max);
        ray = ray & (mask >> (63 - locMax)); // same deal as above
    }

    return ray & ~(ray & colPieces); // doesn't capture own pieces
}

int Board::isKingInCheck(Color color){

    int board = (color == WHITE) ? 5 : 11;
    U64 loc = *boards[board].i;

    U64 rookRays = getRookMove(loc, color);
    U64 bishopRays = getBishopMove(loc, color);

    int otherCol = (board == 5) ? 6 : 0;
    if((rookRays & *boards[otherCol + 3].i)) return 3;
    else if(bishopRays & *boards[otherCol + 2].i) return 2;
    else if ((bishopRays | rookRays) & *boards[otherCol + 4].i) return 4;
        // intersecting with any of the opponent sliding pieces

    U64 knights = *boards[otherCol + 1].i;
    U64 knightFromKing = getKnightMove(loc, color);
    if(knights & knightFromKing) return true; 
    // knight putting in check

    U64 kingMove = 0;
    unsigned int sq = getLSLoc(loc);

    // shifts the king mask
    kingMove = ((sq > 9) * (kingMask << (sq - 9))) | 
        ((sq <= 9) * (kingMask >> (9 - sq)));

    // masks out overflow bits
    sq = ((sq & 0x7) >> 2);
    kingMove &= (sq * (~abFile)) | (!sq * (~ghFile));
    if(kingMove & *boards[otherCol + 5].i) return 5;
    // king putting king in check
    
    U64 pawns = *boards[otherCol].i;
    U64 mask = 0;
    if(otherCol == 0){
        mask |= ((pawns << 7) & ~hFile) | ((pawns << 9) & ~aFile);
    }else{
        mask |= ((pawns >> 7) & ~aFile) | ((pawns >> 9) & ~hFile);
    }
    if(mask & loc) return 6;
    return 0;
    // intersection with pawns if true, otherwise not in check
    // TODO not handling check by enpassant i think
}

Move Board::movePiece(U64 from, Color color, char piece, U64 to){
    // cant go to the same place, or not have a color
    Move m;
    if(to & from || color == NONE) return m;

    char special = 0;
    char castlingRights = '0';

    // handle a capture
    int col;
    if(color == WHITE && (blackPieces & to)){
        // white captured a piece
        blackPieces ^= to; // remove it
        special = 4; // capture
        col = 6;
    }else if(color == BLACK && (whitePieces & to)){
        // black captured a piece
        whitePieces ^= to; // remove it
        special = 4; // capture
        col = 0;
    }
    if(special == 4){
        for(int i = col; i < col + 6; i++){
            if(*boards[i].i & to){
                *boards[i].i ^= to; // remove the piece
                // add to captures
                captures.push({boards[i].piece, boards[i].col});
                break;
            }
            if(i == col + 5) cout << "never captured" << endl;
        }
    }

    int board = getBoard(piece, color);
    // move the piece
    *boards[board].i ^= from;
    *boards[board].i |= to;
    // update piece boards
    if(color == WHITE){
        whitePieces ^= from;
        whitePieces |= to;
    }else{
        blackPieces ^= from;
        blackPieces |= to;
    }

    // deal with special cases with pieces
    switch (piece) {
        case 'p':
            // handle enpassanft
            //if((color == BLACK && (enpassantLoc & whiteSide)) || (color == WHITE && (enpassantLoc & blackSide))){
            if(to == enpassantLoc){
                // has to be a capture by enpassant
                special = 5;
                if(color == WHITE){
                    // remove the captured pawn
                    *boards[board + 6].i ^= (to >> 8);
                    blackPieces ^= (to >> 8); // update mask
                    captures.push({'p', BLACK});
                    enpassantLoc = 0;
                }else{
                    // remove the captured pawn
                    *boards[board - 6].i ^= (to << 8);
                    whitePieces ^= (to << 8); // update mask
                    captures.push({'p', WHITE});
                    enpassantLoc = 0;

                }
            }
           // }

            // generate enpassant squares
            else if(to == (from << 16)) enpassantLoc = (from << 8);
            else if(to == (from >> 16)) enpassantLoc = (from >> 8);
            else enpassantLoc = 0;

            // pawn promo
            if(to & rank1 || to & rank8) pawnPromo = to;
            else pawnPromo = 0;
            break;
        case 'k':
            pawnPromo = 0;
            enpassantLoc = 0; // not a pawn move
            // handle castling
            if(color == WHITE && (whiteCastleQueen || whiteCastleKing)){
                if(whiteCastleKing && whiteCastleQueen) castlingRights = 'W';
                else if(whiteCastleKing) castlingRights = 'K';
                else castlingRights = 'Q';
                whiteCastleQueen = 0;
                whiteCastleKing = 0;
            }else if(color == BLACK && (blackCastleKing || blackCastleQueen)){
                if(blackCastleKing && blackCastleQueen) castlingRights = 'B';
                else if(blackCastleKing) castlingRights = 'k';
                else castlingRights = 'q';
                blackCastleQueen = 0;
                blackCastleKing = 0;
            }
            if(to == from << 2){
                // kingside
                special = 2;

                // just need to move rook, already moved king
                if(color == WHITE){
                    *boards[3].i ^= (U64)0x80;
                    whitePieces ^= (U64)0x80;
                    *boards[3].i |= (U64)0x20;
                    whitePieces |= (U64)0x20;
                }else{
                    *boards[9].i ^= 0x8000000000000000;
                    blackPieces ^= 0x8000000000000000;
                    *boards[9].i |= 0x2000000000000000;
                    blackPieces |= 0x2000000000000000;
                }
            }else if(to == from >> 2){
                special = 3;
                // castling queenside 
                if(color == WHITE){
                    *boards[3].i ^= (U64)0x1;
                    whitePieces ^= (U64)0x1;
                    *boards[3].i |= (U64)0x8;
                    whitePieces |= (U64)0x8;
                }else{
                    *boards[9].i ^= 0x0100000000000000;
                    blackPieces ^= 0x0100000000000000;
                    *boards[9].i |= 0x0800000000000000;
                    blackPieces |= 0x0800000000000000;
                }
            }
            break;
        case 'r':
            enpassantLoc = 0; // not a pawn move
            pawnPromo = 0;
            // handle castling rights
            if(color == WHITE){
                if(from == 0x0000000000000001 && whiteCastleQueen){
                    whiteCastleQueen = 0;
                    castlingRights = 'Q';
                }
                if(from == 0x0000000000000080 && whiteCastleKing){
                    whiteCastleKing = 0;
                    castlingRights = 'K';
                }
            }else{
                if(from == 0x0100000000000000 && blackCastleQueen){
                    blackCastleQueen = 0;
                    castlingRights = 'q';
                } 
                if(from == 0x8000000000000000 && blackCastleKing){
                    blackCastleKing = 0;
                    castlingRights = 'k';
                }
            }
            break;
        default:
            enpassantLoc = 0; // not a pawn move
            pawnPromo = 0;
    }

    // add to move stack
    moves.push({from, to, special, piece, color, castlingRights});

    // update moves
    halfMoves++;
    if(turn == WHITE) turn = BLACK;
    else{
        turn = WHITE;
        fullMoves++;
    }
    return moves.top();
}
void Board::movePiece(Move move){
    if(move.to == 0) return;
    int board = getBoard(move.piece, move.color);

    // add to moves
    moves.push(move);

    // move piece
    *boards[board].i ^= move.from;
    *boards[board].i |= move.to;

    // update bitboards
    if(move.color == WHITE){
        whitePieces ^= move.from;
        whitePieces |= move.to;
    }else{
        blackPieces ^= move.from;
        blackPieces |= move.to;
    }

    // pawn special moves
    if(move.piece == 'p'){
        // generate enpassant squares
        if(move.to == (move.from << 16)) enpassantLoc = (move.from << 8);
        else if(move.to == (move.from >> 16)) enpassantLoc = (move.from >> 8);
        else enpassantLoc = 0;

        if(move.to & rank1 || move.to & rank8) pawnPromo = move.to;
        else pawnPromo = 0;
    }else{
        enpassantLoc = 0;
        pawnPromo = 0;
    }

    if(move.special == 2){
        // kingside castle
        if(move.color == WHITE){
            *boards[3].i ^= (U64)0x80;
            whitePieces ^= (U64)0x80;
            *boards[3].i |= (U64)0x20;
            whitePieces |= (U64)0x20;
        }else{
            *boards[9].i ^= 0x8000000000000000;
            blackPieces ^= 0x8000000000000000;
            *boards[9].i |= 0x2000000000000000;
            blackPieces |= 0x2000000000000000;
        }
    }else if(move.special == 3){
        // queenside castle
        if(move.color == WHITE){
            *boards[3].i ^= (U64)0x1;
            whitePieces ^= (U64)0x1;
            *boards[3].i |= (U64)0x8;
            whitePieces |= (U64)0x8;
        }else{
            *boards[9].i ^= 0x0100000000000000;
            blackPieces ^= 0x0100000000000000;
            *boards[9].i |= 0x0800000000000000;
            blackPieces |= 0x0800000000000000;
        }
    }else if(move.special == 4){
        // find the capture
        int col;
        if(move.color == WHITE){
            col = 6;
            blackPieces ^= move.to;
        }else{
            col = 0;
            whitePieces ^= move.to;
        }
        for(int i = col; i < col + 6; i++){
            if(*boards[i].i & move.to){
                *boards[i].i ^= move.to; // remove the piece
                // add to captures
                captures.push({boards[i].piece, boards[i].col});
                break;
            }
            if(i == col + 5) cout << "never captured" << endl;
        }
    }else if(move.special == 5){
        // enpassant capture
        if(move.color == WHITE){
            *boards[6].i ^= (move.to >> 8); // remove black pawn
            blackPieces ^= (move.to >> 8);
            captures.push({'p', BLACK});
        }else{
            *boards[0].i ^= (move.to << 8); // remove white pawn
            whitePieces ^= (move.to << 8);
            captures.push({'p', WHITE});
        }
        enpassantLoc = 0;
    }else if(move.special >= 8){ // promotion
        // we moved the pawn earlier, so need to delete it again
        *boards[board].i ^= move.to; // one less pawn
        // dont need to update colored boards because adding a new piece there
        if(move.special >= 12){
            // promo with capture
            int col;
            if(move.color == WHITE){
                col = 0;
                blackPieces ^= move.to;
            }else{
                col = 6;
                whitePieces ^= move.to;
            }
            for(int i = col; i < col + 6; i++){
                if(*boards[i].i & move.to){
                    *boards[i].i ^= move.to; // remove the piece
                    // add to captures
                    captures.push({boards[i].piece, boards[i].col});
                    break;
                }
            }
        }
        switch(move.special){
            case 8:
            case 12:
                // knight
                *boards[board + 1].i ^= move.to;
                break;
            case 9:
            case 13:
                // bishop
                *boards[board + 2].i ^= move.to;
                break;
            case 10:
            case 14:
                // rook
                *boards[board + 3].i ^= move.to;
                break;
            case 11:
            case 15:
                // queen
                *boards[board + 4].i ^= move.to;
        }
    }
    // castling
    switch(move.castleRight){
        case 'K':
            whiteCastleKing = 0;
            break;
        case 'Q':
            whiteCastleQueen = 0;
            break;
        case 'k':
            blackCastleKing = 0;
            break;
        case 'q':
            blackCastleQueen = 0;
            break;
        case 'W':
            whiteCastleQueen = 0;
            whiteCastleKing = 0;
            break;
        case 'B':
            blackCastleQueen = 0;
            blackCastleKing = 0;
            break;
    }
    // update moves
    halfMoves++;
    if(turn == WHITE) turn = BLACK;
    else{
        turn = WHITE;
        fullMoves++;
    }
}
void Board::unMovePiece(){
    // just do everything backwards
    if(moves.size() == 0) return;

    Move move = moves.top(); // last move
    moves.pop();

    // update moves
    halfMoves--;
    if(turn == WHITE){
        turn = BLACK;
        fullMoves--;
    }
    else{
        turn = WHITE;
    }
    
    enpassantLoc = 0;
    pawnPromo = 0;
    int board = getBoard(move.piece, move.color);
    // move the piece back
    *boards[board].i ^= move.to; // delete old piece
    *boards[board].i |= move.from; // put in old place
    // update color masks
    if(move.color == WHITE){
        whitePieces ^= move.to;
        whitePieces |= move.from;
    }else{
        blackPieces ^= move.to;
        blackPieces |= move.from;
    }

    if(move.special == 2){
        // kingside castle
        // just need to move rook
        if(move.color == WHITE){
            *boards[3].i ^= (U64)0x20;
            whitePieces ^= (U64)0x20;
            *boards[3].i |= (U64)0x80;
            whitePieces |= (U64)0x80;
        }else{
            *boards[9].i ^= 0x2000000000000000;
            blackPieces ^= 0x2000000000000000;
            *boards[9].i |= 0x8000000000000000;
            blackPieces |= 0x8000000000000000;
        }
    }else if(move.special == 3){
        // queenside castle
        if(move.color == WHITE){
            *boards[3].i ^= (U64)0x8;
            whitePieces ^= (U64)0x8;
            *boards[3].i |= (U64)0x1;
            whitePieces |= (U64)0x1;
        }else{
            *boards[9].i ^= 0x0800000000000000;
            blackPieces ^= 0x0800000000000000;
            *boards[9].i |= 0x0100000000000000;
            blackPieces |= 0x0100000000000000;
        }
    }else if(move.special == 4){
        // capture
        auto piece = captures.top();
        captures.pop(); // gets last captured piece
        int capBoard = getBoard(piece.first, piece.second);
        *boards[capBoard].i |= move.to;
        if(move.color == WHITE) blackPieces |= move.to;
        else whitePieces |= move.to;
    }else if(move.special == 5){
        // enpassant capture
        auto piece = captures.top();
        captures.pop(); // gets last captured piece
        int capBoard = getBoard(piece.first, piece.second);

        enpassantLoc = move.to; // where the pawn went
        // put the pieces back
        if(piece.second == BLACK){
            *boards[capBoard].i |= (move.to >> 8);
            blackPieces |= (move.to >> 8);
        }
        else{
            *boards[capBoard].i |= (move.to << 8);
            whitePieces |= (move.to << 8);
        }
    }else if(move.special >= 8){
        // promotions
        
        // promo with capture
        if(move.special >= 12){
            auto piece = captures.top();
            captures.pop();
            // get last captured piece
            int capBoard = getBoard(piece.first, piece.second);
            // for normal captures
            *boards[capBoard].i |= move.to; // change captured piece
            if(piece.second == WHITE) whitePieces |= move.to;
            else blackPieces |= move.to;
            // updates bitboards
        }
        
        // this was a pawn move
        *boards[board].i ^= move.to; // un delete old piece(wasnt there)
        int col = (move.color == WHITE) ? 0 : 6;
        // there was a piece there (the promo piece) so dont need to
        // update any bitboards

        // delete the piece that the pawn turned into
        switch(move.special){
            case 8:
            case 12:
                // knight
                *boards[1 + col].i ^= move.to;
                break;
            case 9:
            case 13:
                // bishop
                *boards[2 + col].i ^= move.to;
                break;
            case 10:
            case 14:
                // rook
                *boards[3 + col].i ^= move.to;
                break;
            case 11:
            case 15:
                // queen
                *boards[4 + col].i ^= move.to;
                break;
        }
    }
    // update castling rights
    switch (move.castleRight) {
        case 'K':
            whiteCastleKing = 1;
            break;
        case 'Q':
            whiteCastleQueen = 1;
            break;
        case 'k':
            blackCastleKing = 1;
            break;
        case 'q':
            blackCastleQueen = 1;
            break;
        case 'W':
            whiteCastleQueen = 1;
            whiteCastleKing = 1;
            break;
        case 'B':
            blackCastleQueen = 1;
            blackCastleKing = 1;
            break;
    }
}
void Board::promotePawn(U64 loc, Color color, char to){
    // loc is the pawn to promote
    int board = 0;
    if(color == BLACK) board = 6;
    *boards[board].i ^= loc; // removes the pawn
    int cap = 0;

    auto move = moves.top();
    if(move.special & 4) int cap = 4;
    moves.pop();
    switch (to) {
        case 'n':
            *boards[board + 1].i ^= loc; // adds the piece
            move.special = 8 + cap;
            break;
        case 'b':
            *boards[board + 2].i ^= loc;
            move.special = 9 + cap;
            break;
        case 'r':
            *boards[board + 3].i ^= loc;
            move.special = 10 + cap;
            break;
        case 'q':
            *boards[board + 4].i ^= loc;
            move.special = 11 + cap;
            break;
    }
    moves.push(move); // updates the move
}
int Board::isGameOver(){
    int state = 0; // 0 means still playing
    // 1 = white wins by checkmate
    // 2 = black wins by checkmate
    // 3 = stalemate
    // 4 = insufficient material
    // 5 = 50 move rule
    // 6 = stalemate by repitition

    // no kings left
    if(*boards[5].i == 0) return 2;
    if(*boards[11].i == 0) return 1;

    U64 pieces = blackPieces | whitePieces; // doesn't include kings
    pieces ^= *boards[5].i;
    pieces ^= *boards[11].i; // removes both kings

    // insufficint material
    if(pieces == 0) return 4; // just kings
    // kings + 1 bishop
    // isolates the LSB and gets rid of it to check for only 1 piece
    if((pieces == *boards[2].i && 
            (*boards[2].i ^ (*boards[2].i & -*boards[2].i)) == 0) || 
        (pieces == *boards[8].i && 
            (*boards[8].i ^ (*boards[8].i & -*boards[8].i)) == 0)) return 4; 

    // kings + 1 knight
    // kings + 2 knights of same color
    if(pieces == *boards[1].i){
        U64 temp = *boards[1].i;
        temp = temp ^ (temp & -temp);
        if(temp == 0) return 4;
        temp = temp ^ (temp & -temp);
        if(temp == 0) return 4;
    }
    if(pieces == *boards[7].i){
        U64 temp = *boards[7].i;
        temp = temp ^ (temp & -temp);
        if(temp == 0) return 4;
        temp = temp ^ (temp & -temp);
        if(temp == 0) return 4;
    }

    // kings with each having its own minor piece (as long as not same color bishop)

    // 50 move rule
    if(fullMoves == 50) return 5;
    
    // draw by repitition
    if(moves.empty()) return state;
    Move top = moves.top();
    moves.pop();
    if(moves.empty()){
        moves.push(top);
        return state;
    }
    Move second = moves.top();
    moves.pop();
    if(moves.empty()){
        moves.push(second);
        moves.push(top);
        return state;
    }
    if(top == moves.top()){
        // second move in a row
        moves.pop();
        if(moves.empty()){
            moves.push(top);
            moves.push(second);
            moves.push(top);
            return state;
        }
        if(second == moves.top()){
            moves.pop();
            if(top == moves.top()){
                // third move
                return 6;
            }
            moves.push(second);
        }
        moves.push(top);
    }
    moves.push(second);
    moves.push(top);


    return state;
}

int Board::getBoard(char piece, Color color){
    int board = 0;
    switch (piece){
        case 'p':
            board = 0;
            break;
        case 'n':
            board = 1;
            break;
        case 'b':
            board = 2;
            break;
        case 'r':
            board = 3;
            break;
        case 'q':
            board = 4;
            break;
        case 'k':
            board = 5;
            break;
    }
    return board + (6 * (color == BLACK));
}


/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param mask bitboard to scan
 * @precondition mask != 0
 * @return index (0..63) of least significant one bit
 */
int Board::getLSLoc(U64 mask) {
   const U64 debruijn64 = U64(0x03f79d71b4cb0a89);
   assert (mask != 0);
   return index64[((mask & -mask) * debruijn64) >> 58];
}


std::vector<Move> Board::getAllMoves(){
    std::vector<Move> newMoves;

    for(int i = 0; i < 12; i++){
        if(boards[i].col != turn) continue;
        // loop through each bitboard
        U64 locMask = *boards[i].i;
        while(locMask != 0){
            U64 loc = (locMask & -locMask); // isolates least significant bit
            locMask ^= loc; // remove it

            // there is a piece at loc
            Move moves[256];
            U64 mask = generateMoves(loc, boards[i].piece, boards[i].col, true, moves);
            // has all moves of this piece
            if(mask == 0) continue;
            for(int i = 0; i < mask; i++){
                if(moves[i].from == 0) continue;
                newMoves.push_back(moves[i]);
            }
            /*
            Move temp;
            temp.from = *boards[i].i & loc;
            temp.piece = boards[i].piece;
            temp.color = boards[i].col;
            while(mask != 0){
                loc = (mask & -mask);
                mask ^= loc; // removes the LSB
                temp.to = loc;
                newMoves.push_back(temp);
            }
            */
        }
    }

    return newMoves;
}
U64 Board::Perft(int depth){
    if(depth == 0) return 1;

    U64 moves = 0;
    std::vector<Move> possMoves = getAllMoves();
    for(int i = 0; i < possMoves.size(); i++){
        //movePiece(possMoves[i].from, possMoves[i].color, possMoves[i].piece, possMoves[i].to);
        movePiece(possMoves[i]);
        if(!isGameOver() && turn != NONE) moves += Perft(depth - 1);
        unMovePiece();
    }
    
    return moves;
}
