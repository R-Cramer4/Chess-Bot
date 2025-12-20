#include "Board.hpp"
#include "Masks.hpp"
#include <cassert>

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
                    if(moves[k].to == (loc << 2)){
                        // need to swap
                        moves[k] = moves[i - 1];
                        moves[i - 1].from = 0;
                        i--;
                    }

                }
            }
        }
        if(mask & (loc >> 2)){
            if(c || !(mask & (loc >> 1))){
                // cant castle due to bing in check or cant move one to the left
                mask ^= (loc >> 2); // cant castle in check
                // takes out of the list
                for(int k = 0; k < i; k++){
                    if(moves[k].to == (loc >> 2)){
                        // need to swap
                        moves[k] = moves[i - 1];
                        moves[i - 1].from = 0;
                        i--;
                    }
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
                    i++;
                }
            }
        }
    }
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

    U64 pieces = whitePieces | blackPieces;
    // castling
    if(color == WHITE){
        mask |= (((pieces & WKPieces) == 0) && whiteCastleKing)
            * (loc << 2);
        mask |= (((pieces & WQPieces) == 0) && whiteCastleQueen)
            * (loc >> 2);
    }else{
        mask |= (((pieces & BKPieces) == 0) && blackCastleKing)
            * (loc << 2);
        mask |= (((pieces & BQPieces) == 0) && blackCastleQueen)
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

    pieces ^= loc; // holds all pieces except loc
    pieces &= ray; // holds all pieces intersecting the ray

    int intLoc = getLSLoc(loc);
    U64 pieceSq; // compared to loc
    U64 min = 0;
    U64 max = 0;
    U64 mask = ((U64)1 << intLoc) - 1;
    U64 posMask = pieces & ~(mask); // all bits after loc
    U64 negMask = pieces & mask; // bits below loc

    // locMin is most significant less than sq
    // locMax is least significant greater than sq
    mask = FullBoard;

    if(negMask != 0){
        while(negMask != 0){
            pieceSq = (negMask & -negMask); // isolate LSB
            negMask ^= pieceSq; // remove it
            min = pieceSq;
        }
        locMin = getLSLoc(min);
        // shifts to the right so everything more significant than locMax is a 0
        ray &= (mask << locMin);
    }
    if(posMask != 0){
        locMax = getLSLoc(posMask);
        ray &= (mask >> (63 - locMax)); // same deal as above
    }

    return ray;
}

int Board::isKingInCheck(Color color){

    U64 loc, pawns, knights, bishops, rooks, queens, otherKing;
    if (color == WHITE) {
        loc = whiteKing;
        pawns = blackPawns;
        knights = blackKnights;
        bishops = blackBishops;
        rooks = blackRooks;
        queens = blackQueens;
        otherKing = blackKing;
    } else {
        loc = blackKing;
        pawns = whitePawns;
        knights = whiteKnights;
        bishops = whiteBishops;
        rooks = whiteRooks;
        queens = whiteQueens;
        otherKing = whiteKing;
    }

    U64 rookRays = getRookMove(loc, color);
    U64 bishopRays = getBishopMove(loc, color);

    if((rookRays & rooks)) return 3;
    else if(bishopRays & bishops) return 2;
    else if ((bishopRays | rookRays) & queens) return 4;
        // intersecting with any of the opponent sliding pieces

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
    if(kingMove & otherKing) return 5;
    // king putting king in check
    
    U64 mask = 0;
    if (color == BLACK) {
        mask |= ((pawns << 7) & ~hFile) | ((pawns << 9) & ~aFile);
    } else {
        mask |= ((pawns >> 7) & ~aFile) | ((pawns >> 9) & ~hFile);
    }
    if(mask & loc) return 6;

    return 0;
    // intersection with pawns if true, otherwise not in check
}

char Board::handleCapture(U64 to, Color color, bool handleCastle) {
    char castlingRights = 0;
    // assert(whitePieces == getWhitePieces());
    // assert(blackPieces == getBlackPieces());
    if (color == WHITE) {
        // capturing black pieces
        blackPieces ^= to; // remove it
        // else if blocks because you cant capture multiple pieces
        if (blackPawns & to) {
            blackPawns ^= to;
            captures.push({'p', BLACK});
        }
        if (blackKnights & to) {
            blackKnights ^= to;
            captures.push({'n', BLACK});
        }
        if (blackBishops & to) {
            blackBishops ^= to;
            captures.push({'b', BLACK});
        }
        if (blackRooks & to) {
            blackRooks ^= to;
            captures.push({'r', BLACK});
            if (handleCastle) {
                if (blackCastleKing && (to == BKRook)) {
                    castlingRights += 0b0010;
                    blackCastleKing = 0;
                } else if (blackCastleQueen && (to == BQRook)) {
                    castlingRights += 0b0001;
                    blackCastleQueen = 0;
                }
            }
        }
        if (blackQueens & to) {
            blackQueens ^= to;
            captures.push({'q', BLACK});
        }
        if (blackKing & to) {
            blackKing ^= to;
            captures.push({'k', BLACK});
        }
    } else {
        whitePieces ^= to; // remove it
        if (whitePawns & to) {
            whitePawns ^= to;
            captures.push({'p', WHITE});
        }
        if (whiteKnights & to) {
            whiteKnights ^= to;
            captures.push({'n', WHITE});
        }
        if (whiteBishops & to) {
            whiteBishops ^= to;
            captures.push({'b', WHITE});
        } 
        if (whiteRooks & to) {
            whiteRooks ^= to;
            captures.push({'r', WHITE});
            if (handleCastle) {
                if (whiteCastleKing && (to == WKRook)){
                    castlingRights += 0b1000;
                    whiteCastleKing = 0;
                } else if (whiteCastleQueen && (to == WQRook)){
                    castlingRights += 0b0100;
                    whiteCastleQueen = 0;
                }
            }
        }
        if (whiteQueens & to) {
            whiteQueens ^= to;
            captures.push({'q', WHITE});
        }
        if (whiteKing & to) {
            whiteKing ^= to;
            captures.push({'k', WHITE});
        }
    }
    return castlingRights;
}
Move Board::movePiece(U64 from, Color color, char piece, U64 to){
    // cant go to the same place, or not have a color
    Move m;
    if(to & from || color == NONE) return m;

    char special = 0;
    char castlingRights = 0;
    U64 oldEnpassant = enpassantLoc;

    // handle a capture
    if (color == WHITE && (blackPieces & to)){
        // white captured a piece
        special = 4; // capture
    } else if (color == BLACK && (whitePieces & to)){
        // black captured a piece
        special = 4; // capture
    }
    if (special == 4){
        castlingRights += handleCapture(to, color, true);
    }

    U64 *board = getBoard(piece, color);
    // move the piece
    *board ^= from;
    *board |= to;
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
            // handle enpassant
            if (to == enpassantLoc) {
                // has to be a capture by enpassant
                special = 5;
                if (color == WHITE){
                    // remove the captured pawn
                    blackPawns ^= (to >> 8);
                    blackPieces ^= (to >> 8); // update mask
                    captures.push({'p', BLACK});
                    enpassantLoc = 0;
                } else {
                    // remove the captured pawn
                    whitePawns ^= (to << 8);
                    whitePieces ^= (to << 8); // update mask
                    captures.push({'p', WHITE});
                    enpassantLoc = 0;

                }
            }
            // generate enpassant squares
            else if (to == (from << 16)) enpassantLoc = (from << 8);
            else if (to == (from >> 16)) enpassantLoc = (from >> 8);
            else enpassantLoc = 0;

            // pawn promo
            if (to & rank1 || to & rank8) pawnPromo = to;
            else pawnPromo = 0;
            break;
        case 'k':
            pawnPromo = 0;
            enpassantLoc = 0; // not a pawn move
            // handle castling
            if (color == WHITE) {
                // branchless
                castlingRights += whiteCastleKing * 0b1000;
                castlingRights += whiteCastleQueen * 0b0100;
                whiteCastleQueen = 0;
                whiteCastleKing = 0;
            } else if (color == BLACK){
                castlingRights += blackCastleKing * 0b0010;
                castlingRights += blackCastleQueen * 0b0001;
                blackCastleQueen = 0;
                blackCastleKing = 0;
            }
            if (to == from << 2){
                // kingside
                special = 2;

                // just need to move rook, already moved king
                if (color == WHITE){
                    whiteRooks ^= (U64)0x80;
                    whitePieces ^= (U64)0x80;
                    whiteRooks |= (U64)0x20;
                    whitePieces |= (U64)0x20;
                } else {
                    blackRooks ^= 0x8000000000000000;
                    blackPieces ^= 0x8000000000000000;
                    blackRooks |= 0x2000000000000000;
                    blackPieces |= 0x2000000000000000;
                }
            }else if(to == from >> 2){
                special = 3;
                // castling queenside 
                if(color == WHITE){
                    whiteRooks ^= (U64)0x1;
                    whitePieces ^= (U64)0x1;
                    whiteRooks |= (U64)0x8;
                    whitePieces |= (U64)0x8;
                }else{
                    blackRooks ^= 0x0100000000000000;
                    blackPieces ^= 0x0100000000000000;
                    blackRooks |= 0x0800000000000000;
                    blackPieces |= 0x0800000000000000;
                }
            }
            break;
        case 'r':
            enpassantLoc = 0; // not a pawn move
            pawnPromo = 0;
            // handle castling rights
            if(color == WHITE){
                if(from == WQRook && whiteCastleQueen){
                    whiteCastleQueen = 0;
                    castlingRights += 0b0100;
                }
                if(from == WKRook && whiteCastleKing){
                    whiteCastleKing = 0;
                    castlingRights += 0b1000;
                }
            }else{
                if(from == BQRook && blackCastleQueen){
                    blackCastleQueen = 0;
                    castlingRights += 0b0001;
                } 
                if(from == BKRook && blackCastleKing){
                    blackCastleKing = 0;
                    castlingRights += 0b0010;
                }
            }
            break;
        default:
            enpassantLoc = 0; // not a pawn move
            pawnPromo = 0;
    }

    // add to move stack
    moves.push({from, to, special, piece, color, castlingRights, oldEnpassant});

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
    U64 *board = getBoard(move.piece, move.color);

    // add to moves
    moves.push(move);

    // move piece
    *board ^= move.from;
    *board |= move.to;

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

    if (move.special == 2) {
        // kingside castle
        if(move.color == WHITE){
            whiteRooks ^= (U64)0x80;
            whitePieces ^= (U64)0x80;
            whiteRooks |= (U64)0x20;
            whitePieces |= (U64)0x20;
        }else{
            blackRooks ^= 0x8000000000000000;
            blackPieces ^= 0x8000000000000000;
            blackRooks |= 0x2000000000000000;
            blackPieces |= 0x2000000000000000;
        }
    } else if (move.special == 3) {
        // queenside castle
        if (move.color == WHITE) {
            whiteRooks ^= (U64)0x1;
            whitePieces ^= (U64)0x1;
            whiteRooks |= (U64)0x8;
            whitePieces |= (U64)0x8;
        } else {
            blackRooks ^= 0x0100000000000000;
            blackPieces ^= 0x0100000000000000;
            blackRooks |= 0x0800000000000000;
            blackPieces |= 0x0800000000000000;
        }
    } else if (move.special == 4) {
        // find the capture
        handleCapture(move.to, move.color, false);

    } else if (move.special == 5) {
        // enpassant capture
        if(move.color == WHITE){
            blackPawns ^= (move.to >> 8); // remove black pawn
            blackPieces ^= (move.to >> 8);
            captures.push({'p', BLACK});
        }else{
            whitePawns ^= (move.to << 8); // remove white pawn
            whitePieces ^= (move.to << 8);
            captures.push({'p', WHITE});
        }
        enpassantLoc = 0;
    } else if (move.special >= 8) { // promotion
        // we moved the pawn earlier, so need to delete it again
        *board ^= move.to; // one less pawn
        // dont need to update colored boards because adding a new piece there
        if(move.special >= 12){
            // promo with capture
            handleCapture(move.to, move.color, false);
        }
        switch(move.special){
            case 8:
            case 12:
                // knight
                if (move.color == WHITE) {
                    whiteKnights ^= move.to;
                } else {
                    blackKnights ^= move.to;
                }
                break;
            case 9:
            case 13:
                // bishop
                if (move.color == WHITE) {
                    whiteBishops ^= move.to;
                } else {
                    blackBishops ^= move.to;
                }
                break;
            case 10:
            case 14:
                // rook
                if (move.color == WHITE) {
                    whiteRooks ^= move.to;
                } else {
                    blackRooks ^= move.to;
                }
                break;
            case 11:
            case 15:
                // queen
                if (move.color == WHITE) {
                    whiteQueens ^= move.to;
                } else {
                    blackQueens ^= move.to;
                }
                break;
        }
    }

    // castling
    // if it was 1 to begin with (LHS) and it was removed (RHS) == false
    // if it was 0 to begin with (LHS) auto false
    whiteCastleQueen = whiteCastleQueen && !(move.castleRight & 0b0100);
    whiteCastleKing = whiteCastleKing && !(move.castleRight & 0b1000);
    blackCastleQueen = blackCastleQueen && !(move.castleRight & 0b0001);
    blackCastleKing = blackCastleKing && !(move.castleRight & 0b0010);

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
    
    enpassantLoc = move.enpassant;
    pawnPromo = 0;
    U64 *board = getBoard(move.piece, move.color);
    // move the piece back
    *board ^= move.to; // delete old piece
    *board |= move.from; // put in old place
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
            whiteRooks ^= (U64)0x20;
            whitePieces ^= (U64)0x20;
            whiteRooks |= (U64)0x80;
            whitePieces |= (U64)0x80;
        }else{
            blackRooks ^= 0x2000000000000000;
            blackPieces ^= 0x2000000000000000;
            blackRooks |= 0x8000000000000000;
            blackPieces |= 0x8000000000000000;
        }
    }else if(move.special == 3){
        // queenside castle
        if(move.color == WHITE){
            whiteRooks ^= (U64)0x8;
            whitePieces ^= (U64)0x8;
            whiteRooks |= (U64)0x1;
            whitePieces |= (U64)0x1;
        }else{
            blackRooks ^= 0x0800000000000000;
            blackPieces ^= 0x0800000000000000;
            blackRooks |= 0x0100000000000000;
            blackPieces |= 0x0100000000000000;
        }
    }else if(move.special == 4){
        // capture
        auto piece = captures.top();
        captures.pop(); // gets last captured piece
        U64 *capBoard = getBoard(piece.first, piece.second);
        *capBoard |= move.to;
        if(move.color == WHITE) blackPieces |= move.to;
        else whitePieces |= move.to;
    }else if(move.special == 5){
        // enpassant capture
        auto piece = captures.top();
        captures.pop(); // gets last captured piece
        U64 *capBoard = getBoard(piece.first, piece.second);

        enpassantLoc = move.to; // where the pawn went
        // put the pieces back
        if(piece.second == BLACK){
            *capBoard |= (move.to >> 8);
            blackPieces |= (move.to >> 8);
        }
        else{
            *capBoard |= (move.to << 8);
            whitePieces |= (move.to << 8);
        }
    }else if(move.special >= 8){
        // promotions
        
        // promo with capture
        if(move.special >= 12){
            auto piece = captures.top();
            captures.pop();
            // get last captured piece
            U64 *capBoard = getBoard(piece.first, piece.second);
            // for normal captures
            *capBoard |= move.to; // change captured piece
            if(piece.second == WHITE) whitePieces |= move.to;
            else blackPieces |= move.to;
            // updates bitboards
        }
        
        // this was a pawn move
        *board ^= move.to; // un delete old piece(wasnt there)
        // there was a piece there (the promo piece) so dont need to
        // update any bitboards

        // delete the piece that the pawn turned into
        switch(move.special){
            case 8:
            case 12:
                // knight
                if (move.color == WHITE) {
                    whiteKnights ^= move.to;
                } else {
                    blackKnights ^= move.to;
                }
                break;
            case 9:
            case 13:
                // bishop
                if (move.color == WHITE) {
                    whiteBishops ^= move.to;
                } else {
                    blackBishops ^= move.to;
                }
                break;
            case 10:
            case 14:
                // rook
                if (move.color == WHITE) {
                    whiteRooks ^= move.to;
                } else {
                    blackRooks ^= move.to;
                }
                break;
            case 11:
            case 15:
                // queen
                if (move.color == WHITE) {
                    whiteQueens ^= move.to;
                } else {
                    blackQueens ^= move.to;
                }
                break;
        }
    }
    // castling rights
    // either still true (LHS == true) or was changed (RHS == true)
    whiteCastleQueen = whiteCastleQueen || (move.castleRight & 0b0100);
    whiteCastleKing = whiteCastleKing || (move.castleRight & 0b1000);
    blackCastleQueen = blackCastleQueen || (move.castleRight & 0b0001);
    blackCastleKing = blackCastleKing || (move.castleRight & 0b0010);
}
void Board::promotePawn(U64 loc, Color color, char to){
    // loc is the pawn to promote
    U64 *board = color == WHITE ? &whitePawns : &blackPawns;
    *board ^= loc; // removes the pawn
    int cap = 0;

    auto move = moves.top();
    if(move.special & 4) int cap = 4;
    moves.pop();
    switch (to) {
        case 'n':
            if (color == WHITE)
                whiteKnights ^= loc;
            else
                blackKnights ^= loc;
            move.special = 8 + cap;
            break;
        case 'b':
            if (color == WHITE)
                whiteBishops ^= loc;
            else
                blackBishops ^= loc;
            move.special = 9 + cap;
            break;
        case 'r':
            if (color == WHITE)
                whiteRooks ^= loc;
            else
                blackRooks ^= loc;
            move.special = 10 + cap;
            break;
        case 'q':
            if (color == WHITE)
                whiteQueens ^= loc;
            else
                blackQueens ^= loc;
            move.special = 11 + cap;
            break;
    }
    moves.push(move); // updates the move
}

// if !act, only check if the kings are there
int Board::isGameOver(bool act){
    int state = 0; // 0 means still playing
    // 1 = white wins by checkmate
    // 2 = black wins by checkmate
    // 3 = stalemate
    // 4 = insufficient material
    // 5 = 50 move rule
    // 6 = stalemate by repitition

    // no kings left
    if(whiteKing == 0) return 2;
    if(blackKing == 0) return 1;
    if(!act) return 0;

    U64 pieces = blackPieces | whitePieces; // doesn't include kings
    pieces ^= whiteKing;
    pieces ^= blackKing; // removes both kings

    // insufficint material
    if(pieces == 0) return 4; // just kings
    // kings + 1 bishop
    // isolates the LSB and gets rid of it to check for only 1 piece
    if((pieces == whiteBishops && 
            (whiteBishops ^ (whiteBishops & -whiteBishops)) == 0) || 
        (pieces == blackBishops && 
            (blackBishops ^ (blackBishops & -blackBishops)) == 0)) return 4; 

    // kings + 1 knight
    // kings + 2 knights of same color
    if(pieces == whiteKnights){
        U64 temp = whiteKnights;
        temp = temp ^ (temp & -temp);
        if(temp == 0) return 4;
        temp = temp ^ (temp & -temp);
        if(temp == 0) return 4;
    }
    if(pieces == blackKnights){
        U64 temp = blackKnights;
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

U64 *Board::getBoard(char piece, Color color){
    int board = 0;
    switch (piece){
        case 'p':
            return color == WHITE ? &whitePawns : &blackPawns;
            break;
        case 'n':
            return color == WHITE ? &whiteKnights : &blackKnights;
            break;
        case 'b':
            return color == WHITE ? &whiteBishops : &blackBishops;
            break;
        case 'r':
            return color == WHITE ? &whiteRooks : &blackRooks;
            break;
        case 'q':
            return color == WHITE ? &whiteQueens : &blackQueens;
            break;
        case 'k':
            return color == WHITE ? &whiteKing : &blackKing;
            break;
    }
    return nullptr;
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


void Board::getAllMovesFromBoard(U64 board, char piece, Color color, std::vector<Move> &newMoves) {
    if (color != turn) return;
    
    U64 locMask = board;
    while(locMask != 0){
        U64 loc = (locMask & -locMask); // isolates least significant bit
        locMask ^= loc; // remove it

        // there is a piece at loc
        Move moves[256];
        U64 len = generateMoves(loc, piece, color, true, moves);
        // has all moves of this piece
        if(len == 0) continue;
        for(int i = 0; i < len; i++){
            if(moves[i].from == 0) continue;
            newMoves.push_back(moves[i]);
        }
    }
}
std::vector<Move> Board::getAllMoves(){
    std::vector<Move> newMoves;

    getAllMovesFromBoard(whitePawns, 'p', WHITE, newMoves);
    getAllMovesFromBoard(whiteKnights, 'n', WHITE, newMoves);
    getAllMovesFromBoard(whiteBishops, 'b', WHITE, newMoves);
    getAllMovesFromBoard(whiteRooks, 'r', WHITE, newMoves);
    getAllMovesFromBoard(whiteQueens, 'q', WHITE, newMoves);
    getAllMovesFromBoard(whiteKing, 'k', WHITE, newMoves);

    getAllMovesFromBoard(blackPawns, 'p', BLACK, newMoves);
    getAllMovesFromBoard(blackKnights, 'n', BLACK, newMoves);
    getAllMovesFromBoard(blackBishops, 'b', BLACK, newMoves);
    getAllMovesFromBoard(blackRooks, 'r', BLACK, newMoves);
    getAllMovesFromBoard(blackQueens, 'q', BLACK, newMoves);
    getAllMovesFromBoard(blackKing, 'k', BLACK, newMoves);

    return newMoves;
}
U64 Board::Perft(int depth){
    if(depth == 0) return 1;

    U64 moves = 0;
    std::vector<Move> possMoves = getAllMoves();
    for(int i = 0; i < possMoves.size(); i++){
        movePiece(possMoves[i]);
        int status = isGameOver(false);
        if(!status) moves += Perft(depth - 1); // if there is kings left continue
        else moves++; // can move here, just ends the game
        unMovePiece();
    }
    
    return moves;
}
