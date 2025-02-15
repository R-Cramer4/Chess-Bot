#include "Board.h"
#include <cassert>
#include <iostream>

using namespace std;

Color Board::generateBitBoards(string fen){
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    if(fen == "") fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    U64 rank = 7;
    U64 col = 0;

    string positions = fen.substr(0, fen.find(' '));
    for(int i = 0; i < positions.size(); i++){
        U64 pos = ((U64)1 << ((rank * (U64)8) + col));
        switch(fen[i]){
            case '/':
                rank--;
                col = 0;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                col += fen[i] - 48; // converts to int and adds
                break;
            case 'p': // black pawn
                blackPawns |= pos;
                col++;
                break;
            case 'r':
                blackRooks |= pos;
                col++;
                break;
            case 'n':
                blackKnights |= pos;
                col++;
                break;
            case 'b':
                blackBishops |= pos;
                col++;
                break;
            case 'q':
                blackQueens |= pos;
                col++;
                break;
            case 'k':
                blackKing |= pos;
                col++;
                break;
            case 'P':
                whitePawns |= pos;
                col++;
                break;
            case 'R':
                whiteRooks |= pos;
                col++;
                break;
            case 'N':
                whiteKnights |= pos;
                col++;
                break;
            case 'B':
                whiteBishops |= pos;
                col++;
                break;
            case 'Q':
                whiteQueens |= pos;
                col++;
                break;
            case 'K':
                whiteKing |= pos;
                col++;
                break;
        }
    }
    // other info loop
    string otherInfo = fen.substr(fen.find(' ') + 1);
    switch(otherInfo[0]){
        case 'w':
            turn = WHITE;
            break;
        case 'b':
            turn = BLACK;
            break;
        default:
            cout << otherInfo[0] << " <- got this instead" << endl;
    }
    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    for(int i = 0; i < otherInfo.find(' '); i++){
        switch(otherInfo[i]){
            case 'K':
                whiteCastleKing = true;
                break;
            case 'Q':
                whiteCastleQueen = true;
                break;
            case 'k':
                blackCastleKing = true;
                break;
            case 'q':
                blackCastleQueen = true;
                break;
        }
    }
    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    // holds enpassant square, halfmove clock, fullmove number

    // otherInfo[0] is row
    if(otherInfo[0] != '-'){
        enpassantLoc = (U64)1 << (8 * (otherInfo[0] - 'a'));
        // shifts up rows
        enpassantLoc = enpassantLoc << (otherInfo[1] - '1');
        // shifts over columns
    }

    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    halfMoves = stoi(otherInfo.substr(0, otherInfo.find(' ')));

    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    fullMoves = stoi(otherInfo.substr(0, otherInfo.find(' ')));

    return turn;
}
void Board::reset(std::string fen){
    for(int i = 0; i < 12; i++){
        *boards[i].i = 0;
    }
    colorMask = 0;
    debugMask = 0;

    enpassantLoc = 0;

whiteCastleKing = 1;
    whiteCastleQueen = 1;
    blackCastleKing = 1;
    blackCastleQueen = 1;

    halfMoves = 0;
    fullMoves = 0;

    while(!moves.empty()){
        moves.pop();
    }
    while(!captures.empty()){
        captures.pop();
    }
    generateBitBoards(fen);

}

Board::Board(Board &ref){
    this->whitePawns = ref.whitePawns;
    this->whiteKnights = ref.whiteKnights;
    this->whiteBishops = ref.whiteBishops;
    this->whiteRooks = ref.whiteRooks;
    this->whiteQueens = ref.whiteQueens;
    this->whiteKing = ref.whiteKing;

    this->blackPawns = ref.blackPawns;
    this->blackKnights = ref.blackKnights;
    this->blackBishops = ref.blackBishops;
    this->blackRooks = ref.blackRooks;
    this->blackQueens = ref.blackQueens;
    this->blackKing = ref.blackKing;

    this->enpassantLoc = ref.enpassantLoc;

    this->whiteCastleKing = ref.whiteCastleKing;
    this->whiteCastleQueen = ref.whiteCastleQueen;
    this->blackCastleKing = ref.blackCastleKing;
    this->blackCastleQueen = ref.blackCastleQueen;

    this->halfMoves = ref.halfMoves;
    this->fullMoves = ref.fullMoves;
    this->moves = ref.moves;
    this->captures = ref.captures;

    this->turn = ref.turn;
}
void Board::printLoc(U64 x){
    printf("0x%016llu\n", x);
}

U64 Board::generateMoves(U64 loc, char piece, Color color, bool top){
    // if top, generates pseudo legal moves
    // otherwise it generates all legal moves
    U64 mask = 0;
    switch(piece){
        case 'n':
            // done, no testing
            mask = getKnightMove(loc, color);
            break;
        case 'p':
            // done no testing
            mask = getPawnMove(loc, color);
            break;
        case 'b':
            // done no testing
            mask = getBishopMove(loc, color);
            break;
        case 'r':
            // done no testing
            mask = getRookMove(loc, color);
            break;
        case 'q':
            // done no testing
            mask = getQueenMove(loc, color);
            break;
        case 'k':
            // done no testing
            mask = getKingMove(loc, color);
            break;
    }
    if(!top) return mask;

    int board = getBoard(piece, color);

    // mask holds all of the potential moves

    // go through each move and check if it would leave the king in check

    U64 temp = mask;
    while(temp != 0){
        U64 move = (temp & -temp);
        temp ^= move;
        if(mask & move){
            // this is a valid move
            movePiece(loc, color, piece, move); 
            if(isKingInCheck(color)){
                // our king is in check
                mask ^= move;
            }
            unMovePiece();
        }
    }
    if(piece == 'k'){
        if(mask & (loc << 2)){
            // can castle kinside
            if(!(mask & (loc << 1))){
                // cant move one left
                mask ^= (loc << 2);
            }
        }else if(mask & (loc >> 2)){
            // castle queenside
            if(!(mask & (loc >> 1))){
                // cant move one to the side
                mask ^= (loc >> 2);
            }
        }
    }
    return mask;
}
void Board::movePiece(U64 from, Color color, char piece, U64 newSpot){
    if(newSpot & from || color == NONE) return; // if it is the same spot, or no color
    char special = 0;
    char castlingRights = '0';
    // and we have the spot we are moving to

    // need to check if the newSpot is in the other pieces
    for(int i = 0; i < 12; i++){
        if((*boards[i].i & newSpot) != 0){
            *boards[i].i ^= newSpot; // removes that piece
            special = 4;
            captures.push({boards[i].piece, boards[i].col}); // add to capture list
            break; // exit loop, should be fine
        }
    }

    int board = getBoard(piece, color); // get board piece is on

    // deal with enpassant here, adding and removing squares
    if(piece == 'p'){
        // need to check for capture by enpassant first
        if(color == WHITE && enpassantLoc >= 0x0000000100000000 &&
            (newSpot & enpassantLoc)){
            *boards[board + 6].i ^= (newSpot >> 8);
            captures.push({boards[board + 6].piece, boards[board + 6].col});
            enpassantLoc = 0;
            special = 5;
        }else if(color == BLACK && enpassantLoc < 0x0000000100000000 &&
            (newSpot & enpassantLoc)){
            *boards[board - 6].i ^= (newSpot << 8);
            captures.push({boards[board - 6].piece, boards[board - 6].col});
            enpassantLoc = 0;
            special = 5;
        }
        // if we can shift 16 and get the new spot we add an enpassant square
        // if we can shift back 8 and theres an enpassant square, remove it
        if((from << 16) == newSpot) enpassantLoc = from << 8;
        else if((from >> 16) == newSpot) enpassantLoc = from >> 8;
        else enpassantLoc = 0;
    }else{
        enpassantLoc = 0;
    }


    // handle castling rights

    // king
    if(piece == 'k'){
        if(color == WHITE && (whiteCastleQueen || whiteCastleKing)){
            if(!whiteCastleKing) castlingRights = 'Q';
            else if(!whiteCastleQueen) castlingRights = 'K';
            else castlingRights = 'W';
            this->whiteCastleQueen = 0;
            this->whiteCastleKing = 0;
        }else if(color == BLACK && (blackCastleQueen || blackCastleQueen)){
            if(!blackCastleKing) castlingRights = 'q';
            else if(!blackCastleQueen) castlingRights = 'k';
            else castlingRights = 'B';
            this->blackCastleQueen = 0;
            this->blackCastleKing = 0;
        }
        if(newSpot == from << 2){
            // kingside
            special = 2;

            // just need to move rook, will move king later
            if(color == WHITE){
                *boards[3].i ^= (U64)0x80;
                *boards[3].i |= (U64)0x20;
            }else{
                *boards[9].i ^= 0x8000000000000000;
                *boards[9].i |= 0x2000000000000000;
            }
        }else if(newSpot == from >> 2){
            special = 3;
            // castling queenside 
            if(color == WHITE){
                *boards[3].i ^= (U64)0x1;
                *boards[3].i |= (U64)0x8;
            }else{
                *boards[9].i ^= 0x0100000000000000;
                *boards[9].i |= 0x0800000000000000;
            }
        }
    }
    // rook
    else if(piece == 'r'){
        if(color == WHITE){
            if(from == 0x0000000000000001 && whiteCastleQueen){
                this->whiteCastleQueen = 0;
                castlingRights = 'Q';
            }
            if(from == 0x0000000000000080 && whiteCastleKing){
                this->whiteCastleKing = 0;
                castlingRights = 'K';
            }
        }else{
            if(from == 0x0100000000000000 && blackCastleQueen){
                this->blackCastleQueen = 0;
                castlingRights = 'q';
            } 
            if(from == 0x8000000000000000 && blackCastleKing){
                this->blackCastleKing = 0;
                castlingRights = 'k';
            }
        }
    }

    // move piece
    *boards[board].i ^= from;
    *boards[board].i |= newSpot;
    moves.push({from, newSpot, special, piece, color, castlingRights});

    halfMoves++;
    if(turn == WHITE) turn = BLACK;
    else{
        turn = WHITE;
        fullMoves++;
    } 
    // check if a king was taken, and if so set game over
    if(*boards[5].i == 0 || *boards[11].i == 0){
        turn = NONE;
    }
}
void Board::unMovePiece(){
    // TODO restore castling rights on rook/king move
    if(moves.size() == 0) return;
    Move move = moves.top();
    moves.pop();
    // gets last move

    turn = move.color; // update turn
    halfMoves--;
    if(move.color == BLACK) fullMoves--;

    // find the board and get piece from it
    int board = getBoard(move.piece, move.color);

    // move the piece back
    *boards[board].i ^= move.to; // delete old piece
    *boards[board].i |= move.from; // put in old place

    if(move.special & 4){
        // is some sort of capture
        auto piece = captures.top();
        captures.pop();
        // get last captured piece
        board = getBoard(piece.first, piece.second);
        // for normal captures
        if(move.special != 5) *boards[board].i |= move.to;
        else{
            // enpassant capture
            enpassantLoc = move.to; // this is where the pawn went
            if(piece.second == BLACK) *boards[board].i |= (move.to >> 8);
            else *boards[board].i |= (move.to << 8);
        }
    }else if(move.special == 2){
        // kingside castle
        if(move.color == WHITE){
            *boards[3].i ^= (U64)0x20;
            *boards[3].i |= (U64)0x80;
            whiteCastleKing = true;
        }else{
            *boards[9].i ^= 0x2000000000000000;
            *boards[9].i |= 0x8000000000000000;
            blackCastleKing = true;
        }
    }else if(move.special == 3){
        // queenside castle
        if(move.color == WHITE){
            *boards[3].i ^= (U64)0x8;
            *boards[3].i |= (U64)0x1;
            whiteCastleQueen = true;
        }else{
            *boards[9].i ^= 0x0800000000000000;
            *boards[9].i |= 0x0100000000000000;
            blackCastleQueen = true;
        }
    }else if(move.special >= 8){
        // promo
        // is some sort of capture
        if(move.special >= 12){
            auto piece = captures.top();
            captures.pop();
            // get last captured piece
            board = getBoard(piece.first, piece.second);
            // for normal captures
            *boards[board].i |= move.to; // change captured piece
        }

        *boards[board].i ^= move.to; // un delete old piece(wasnt there)
        int col = 0;
        if(move.color == BLACK) col = 6;
        // delete the actual old piece
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
U64 Board::getRookMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = getLSLoc(loc);

    U64 hori = ((U64)0xff << (sq & 56));
    U64 vert = ((U64)0x0101010101010101 << (sq & 7));
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
U64 Board::getKingMove(U64 loc, Color color){
    U64 mask = 0;
    mask |= ((loc << 1) & ~aFile) | ((loc >> 1) & ~hFile); // side to side
    mask |= (loc << 8) | (loc >> 8); // up and down
    mask |= ((loc << 7) & ~hFile) | ((loc << 9) & ~aFile); // diags
    mask |= ((loc >> 7) & ~aFile) | ((loc >> 9) & ~hFile); // lower diags

    U64 pieces = 0;
    for(int i = 0 ; i < 12; i++){
        if(color == boards[i].col) 
            mask ^= (mask & *boards[i].i); // makes sure this isnt the own colors pieces
        pieces |= *boards[i].i;
    }
    // pieces has what we want for castling
    if(isKingInCheck(color)) return mask;
    if(color == WHITE && (whiteCastleKing || whiteCastleQueen)){
        // cant be in check or castle through/into check
        // no other pieces can be there
        // king moves 2 spaces, and rook jumps to other side
        if(whiteCastleKing && !(pieces & 0x0000000000000060)){
            // can castle
            mask |= loc << 2;
        }else if(whiteCastleQueen && !(pieces & 0x000000000000000e)){
            mask |= loc >> 2;
        }
    }else if(color == BLACK && (blackCastleKing || blackCastleQueen)){

        if(blackCastleKing && !(pieces & 0x6000000000000000)){
            mask |= loc << 2;
        }else if(blackCastleQueen && !(pieces & 0x0e00000000000000)){
            mask |= loc >> 2;
        }
    }
    return mask;
}
U64 Board::getPawnMove(U64 loc, Color color){
    U64 mask = 0;

    if(color == WHITE){
        mask |= (loc << 8); // normal moves
        mask |= ((loc & 0x000000000000ff00) << 16);
        // branchless, if a pawn isnt in in the mask it results in a 0 being shifted
    }else{
        mask |= (loc >> 8);
        mask |= ((loc & 0x00ff000000000000) >> 16);
    }
    U64 attack = 0;
    if(color == WHITE) attack = ((loc << 7) & ~hFile) | ((loc << 9) & ~aFile);
    else attack = ((loc >> 7) & ~hFile) | ((loc >> 9) & ~aFile);

    U64 opponentPieces = 0;
    for(int i = 0 ; i < 12; i++){
        mask ^= (mask & *boards[i].i); // cant capture forwards
        if(boards[i].col != color) opponentPieces |= *boards[i].i;
    }
    // check empassantLoc
    if(color == WHITE && enpassantLoc >= 0x0000000100000000) opponentPieces |= enpassantLoc;
    else if(color == BLACK && enpassantLoc < 0x0000000100000000) opponentPieces |= enpassantLoc;
    // the enpassant loc is on the correct side of the board

    attack = attack & opponentPieces;
    mask |= attack;

    // handling jump error
    if(mask & (loc << 16)){
        // if can move twice
        if(!(mask & (loc << 8))) mask ^= (loc << 16); // cant move twice if cant move once
    }else if(mask & (loc >> 16)){
        // if can move twice
        if(!(mask & (loc >> 8))) mask ^= (loc >> 16); // cant move twice if cant move once
    }
    return mask;
}
U64 Board::getKnightMove(U64 loc, Color color){
    U64 mask = 0;

    mask = mask | ((loc << 17) & ~aFile);
    mask = mask | ((loc << 10) & ~abFile);
    mask = mask | ((loc >> 6) & ~abFile);
    mask = mask | ((loc >> 15) & ~aFile);

    mask = mask | ((loc >> 17) & ~hFile);
    mask = mask | ((loc >> 10) & ~ghFile);
    mask = mask | ((loc << 6) & ~ghFile);
    mask = mask | ((loc << 15) & ~hFile);
    for(int i = (color ? 0 : 6) ; i < (color ? 6 : 12); i++){
        mask ^= (mask & *boards[i].i); // makes sure this isnt the own colors pieces
    }
    return mask;
}
U64 Board::getActualRay(U64 loc, U64 ray, Color color){
    // TODO make more efficient
    // get the most significant 1 that is intersecting the ray while being less significant than loc
    // get the least significant 1 that is intersecting the ray while being more significant than loc

    int locMin = -1;
    int locMax = -1;
    U64 pieces = 0; // holds all pieces
    U64 colPieces = 0;
    for(int i = 0; i < 12; i++){
        pieces |= *boards[i].i;
        if(color == boards[i].col) colPieces |= *boards[i].i;
    }
    pieces ^= loc; // holds all pieces except loc
    pieces &= ray; // holds all pieces intersecting the ray


    U64 pieceSq; // compared to loc
    U64 min = 0;
    U64 max = 0;
    while(pieces != 0){
        pieceSq = (pieces & -pieces); // isolate LSB
        pieces ^= pieceSq; // remove it
        
        if(pieceSq < loc) min = pieceSq;
        else if(max == 0){
            max = pieceSq;
            break;
        }
    }
    if(min != 0) locMin = getLSLoc(min);
    if(max != 0) locMax = getLSLoc(max);


    // locMin is most significant less than sq
    // locMax is least significant greater than sq
    U64 mask = 0xFFFFFFFFFFFFFFFF;
    if(locMax != -1) ray = ray & (mask >> (63 - locMax));
// shifts to the right so everything more significant than locMax is a 0
    if(locMin != -1) ray = ray & (mask << locMin); // same deal

    ray = ray & ~(ray & colPieces); // shouldnt be able to capture own pieces

    return ray;
}

bool Board::isKingInCheck(Color c){
    // this king can only be in check if the last move put it in check
    // what if white just moved and checking if white is in check
    //      the only way this would happen is if this was a revealed pin or 
    //      the king had been in check from the previous black move
    //      white could have captured the checking piece though
    // so in the case we are checking the case of the most recent move
    //      Unmove, and call on this king color
    //      make move again and check for revealed pins by 
    //      generating the queen moves from king pos
    //
    //  other case is the normal one where we are checking if white in check if black just moved
    //      get queen move from the king
    //          this handles revealed pins, check by rook, queen, and bishop if they just moved
    //      still have pawn, knight, king
    //          knight - generate a knight move from the king and check if intersect with
    //              other color knights
    //          pawn - generate that pawns next move and see if it intersects with king
    //          king - shouldn't be able to move but same thing, generate king moves
    //          so all three - generate next move
    //
    //
    // so for either of these cases
    //      generate queen move from king and check for intersection
    //
    //      get last other color move, and generate next move if it was a pawn, knight, or king
    //      if that last move was two moves down, just need to make sure it wasn't captured

    // get the board the king is in
    int loc = 0;
    if(c == boards[11].col) loc = 11;
    else loc = 5;

    // generate queen moves from the king
    U64 rookRays = getRookMove(*boards[loc].i, boards[loc].col);
    U64 bishopRays = getBishopMove(*boards[loc].i, boards[loc].col);
    // have rays, they are intersected with opponent pieces if can reach
    // go through bishop, rook, queen
    int col = 0; // for c's pieces, need for !c's pieces
    if(loc == 5) col = 6;
    
    if((*boards[2 + col].i & bishopRays) ||
        (*boards[3 + col].i & rookRays) ||
        (*boards[4 + col].i & (rookRays | bishopRays))){
        // bishop, rook, queen
        return true; 
        // if returned from here, something was checking and we found it
    }

    U64 mask = 0;
    // because we are checking if the king walked into check
    // so we need to check against all pawns, knights, and the other king
    int otherLoc = (loc + 6) % 12;
    U64 pieceSq = 0;
    Color notC = (c == WHITE ? BLACK : WHITE);
    // need to go through all of the pawns, knights, and the king

    // king
    U64 pieces = *boards[otherLoc].i;
    mask |= ((pieces << 1) & ~aFile) | ((pieces >> 1) & ~hFile); // side to side
    mask |= (pieces << 8) | (pieces >> 8); // up and down
    mask |= ((pieces << 7) & ~hFile) | ((pieces << 9) & ~aFile); // diags
    mask |= ((pieces >> 7) & ~aFile) | ((pieces >> 9) & ~hFile); // lower diags

    // pawns
    pieces = *boards[otherLoc - 5].i;
    while(pieces != 0){
        pieceSq = (pieces & -pieces);
        pieces ^= pieceSq;
        if(notC == WHITE) mask |= ((pieceSq << 7) & ~hFile) | ((pieceSq << 9) & ~aFile);
        else mask |= ((pieceSq >> 7) & ~hFile) | ((pieceSq >> 9) & ~aFile);
    }
    
    // knights
    pieces = *boards[otherLoc - 4].i;
    while(pieces != 0){
        pieceSq = (pieces & -pieces);
        pieces ^= pieceSq;
        mask |= ((pieceSq << 17) & ~aFile);
        mask |= ((pieceSq << 10) & ~abFile);
        mask |= ((pieceSq >> 6) & ~abFile);
        mask |= ((pieceSq >> 15) & ~aFile);

        mask |= ((pieceSq >> 17) & ~hFile);
        mask |= ((pieceSq >> 10) & ~ghFile);
        mask |= ((pieceSq << 6) & ~ghFile);
        mask |= ((pieceSq << 15) & ~hFile);
    }

    return mask & *boards[loc].i; // true if intersection, false otherwise
    
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

const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

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
            U64 mask = generateMoves(loc, boards[i].piece, boards[i].col, true);
            // has all moves of this piece
            if(mask == 0) continue;
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
        }
    }

    return newMoves;
}
U64 Board::Perft(int depth){
    if(depth == 0) return 1;

    U64 moves = 0;
    std::vector<Move> possMoves = getAllMoves();
    for(int i = 0; i < possMoves.size(); i++){
        movePiece(possMoves[i].from, possMoves[i].color, possMoves[i].piece, possMoves[i].to);
        if(turn != NONE) moves += Perft(depth - 1);
        unMovePiece();
    }
    
    return moves;
}
void Board::printBoardState(){
    for(int i = 0; i < 12; i++){
        cout << *boards[i].i << " : " << boards[i].piece << " " << boards[i].col << endl;
    }
    cout << whiteCastleQueen << whiteCastleKing << blackCastleQueen << blackCastleKing << endl;
    cout << halfMoves << " " << fullMoves << endl;
    cout << "Moves: " << moves.size() << endl;
    cout << "Cap: " << captures.size() << endl;
}
