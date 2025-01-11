#include "Board.h"
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
    for(int i = 0; i < 4; i++){
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

    this->selectedPiece.piece = ref.selectedPiece.piece;
    this->selectedPiece.col = ref.selectedPiece.col;
    this->selecPieceLoc = ref.selecPieceLoc;
    
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
int Board::findLoc(U64 x){
    int loc = 0;
    while((x & 1) != 1){
        x = x >> 1;
        loc++;
        if(loc > 63) return 0;
    }
    return loc;
}
void Board::printLoc(U64 x){
    printf("0x%016lu\n", x);
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

    Board next(*this);
    // go through each move and check if it would leave the king in check
    // TODO Pieces cant capture other pieces that are putting the king in check
    for(int i = 0; i < 63; i++){
        U64 move = (U64)1 << i;
        if(mask & move){
            // this is a valid move
            next.movePiece(move); 
            if(next.isKingInCheck(color)){
                // our king is in check
                mask ^= move;
            }
            next.unMovePiece();
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
void Board::movePiece(U64 newSpot){
    if(newSpot & *selectedPiece.i) return;
    if(selectedPiece.col == NONE) return;
    char special = 0;
    // selected piece has the old piece
    // and we have the spot we are moving to

    // need to check if the newSpot is in the other pieces
    for(int i = 0; i < 12; i++){
        if((*boards[i].i & newSpot) != 0){
            *boards[i].i ^= newSpot; // removes that piece
            special += 4;
            captures.push({boards[i].piece, boards[i].col});
        }
    }
    if(special > 4) cout << "Capture is too high" << endl;

    int board = getBoard(selectedPiece.piece, selectedPiece.col);

    // deal with enpassant here, adding and removing squares
    if(selectedPiece.piece == 'p'){
        // need to check for capture by enpassant first
        if(selectedPiece.col == WHITE && enpassantLoc >= 0x0000000100000000 &&
            (newSpot & enpassantLoc)){
            *boards[board + 6].i ^= (newSpot >> 8);
            captures.push({boards[board + 6].piece, boards[board + 6].col});
            enpassantLoc = 0;
            special = 5;
        }else if(selectedPiece.col == BLACK && enpassantLoc < 0x0000000100000000 &&
            (newSpot & enpassantLoc)){
            *boards[board - 6].i ^= (newSpot << 8);
            captures.push({boards[board - 6].piece, boards[board - 6].col});
            enpassantLoc = 0;
            special = 5;
        }
        // if we can shift 16 and get the new spot we add an enpassant square
        // if we can shift back 8 and theres an enpassant square, remove it
        if((*selectedPiece.i << 16) == newSpot) enpassantLoc = *selectedPiece.i << 8;
        else if((*selectedPiece.i >> 16) == newSpot) enpassantLoc = *selectedPiece.i >> 8;
        else enpassantLoc = 0;
    }else{
        enpassantLoc = 0;
    }


    // handle castling rights

    // king
    if(selectedPiece.piece == 'k'){
        if(selectedPiece.col == WHITE){
            this->whiteCastleQueen = 0;
            this->whiteCastleKing = 0;
        }else{
            this->blackCastleQueen = 0;
            this->blackCastleKing = 0;
        }
        if(newSpot == *selectedPiece.i << 2){
            // kingside
            special = 2;

            // just need to move rook, will move king later
            if(selectedPiece.col == WHITE){
                *boards[3].i ^= (U64)0x80;
                *boards[3].i |= (U64)0x20;
            }else{
                *boards[9].i ^= 0x8000000000000000;
                *boards[9].i |= 0x2000000000000000;
            }
        }else if(newSpot == *selectedPiece.i >> 2){
            special = 3;
            // castling queenside 
            if(selectedPiece.col == WHITE){
                *boards[3].i ^= (U64)0x1;
                *boards[3].i |= (U64)0x8;
            }else{
                *boards[9].i ^= 0x0100000000000000;
                *boards[9].i |= 0x0800000000000000;
            }
        }
    }
    // rook
    else if(selectedPiece.piece == 'r'){
        if(selectedPiece.col == WHITE){
            if(*selectedPiece.i == 0x0000000000000001 && whiteCastleQueen) 
                this->whiteCastleQueen = 0;
            if(*selectedPiece.i == 0x0000000000000080 && whiteCastleKing)
                this->whiteCastleKing = 0;
        }else{
            if(*selectedPiece.i == 0x0100000000000000 && blackCastleQueen) 
                this->blackCastleQueen = 0;
            if(*selectedPiece.i == 0x8000000000000000 && blackCastleKing)
                this->blackCastleKing = 0;
        }
    }

    // move piece
    *boards[board].i ^= *selectedPiece.i;
    *boards[board].i |= newSpot;
    moves.push({*selectedPiece.i, newSpot, special, selectedPiece.piece, selectedPiece.col});

    halfMoves++;
    if(turn == WHITE) turn = BLACK;
    else{
        turn = WHITE;
        fullMoves++;
    } 
}
void Board::unMovePiece(){
    if(moves.size() == 0) return;
    Move move = moves.top();
    moves.pop();
    // gets last move

    turn = move.color; // update turn
    halfMoves--;
    if(move.color == WHITE) fullMoves--;

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
}
U64 Board::getRookMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = findLoc(loc);

    U64 hori = ((U64)0xff << (sq & 56));
    U64 vert = ((U64)0x0101010101010101 << (sq & 7));
    mask |= getActualRay(loc, hori, color);
    mask |= getActualRay(loc, vert, color);

    return mask;
}
U64 Board::getBishopMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = findLoc(loc);

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
    if(color == WHITE){
        if(isKingInCheck(WHITE)) return mask;
        // cant be in check or castle through/into check
        // no other pieces can be there
        // king moves 2 spaces, and rook jumps to other side
        if(whiteCastleKing && !(pieces & 0x0000000000000060)){
            // can castle
            mask |= loc << 2;
        }else if(whiteCastleQueen && !(pieces & 0x000000000000000e)){
            mask |= loc >> 2;
        }
    }else if(color == BLACK){
        if(isKingInCheck(BLACK)) return mask;

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
    int sq = findLoc(loc);
    for(int i = 0; i < 64; i++){
        if(pieces >> i & 1){
            // has a 1 here
            if(i < sq) locMin = i;
            else if(locMax == -1) locMax = i;
        }
    }
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
    if(this->moves.size() < 2) return false;
    // checks for the color that we pass it
    // returns the color of one of the kings if it is in check

    // both kings can never be in check at same time
    // so the only way a king would be in chekc is if the last move put it in check

    // first check last piece moved
    Move top = moves.top(); // last piece
    this->moves.pop();
    Move last = moves.top(); // piece before
    this->moves.push(top); // fix moves


    if(c != top.color){
        last = top; // check opponents king
    }
    U64 nextMoves = generateMoves(last.to, last.piece, last.color, false);
    int loc = 0;
    if(c == boards[11].col) loc = 11;
    else loc = 5;
    if(*boards[loc].i & nextMoves) return true;
    // checks if there is an intersection between the king we are checking and the moves we generated
    // this works

    // what about revealed pins
    // only need to worry about rooks, bishops, and queens
    // can generate all sliding moves from my king
    // if i mask my own pieces and then if it intersects with sliding pieces who can make that move, the king is in check

    U64 rookRays = getRookMove(*boards[loc].i, boards[loc].col);
    U64 bishopRays = getBishopMove(*boards[loc].i, boards[loc].col);
    // have rays, they are intersected with opponent pieces if can reach
    // go through bishop, rook, queen
    int col = 0; // for c's pieces, need for !c's pieces
    if(loc == 5) col = 6;
    
    if((*boards[2 + col].i & bishopRays) ||
        (*boards[3 + col].i & rookRays) ||
        (*boards[4 + col].i & (rookRays | bishopRays))){
        // bishop, rook, king
        return true; 
    } 
    return false;
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
        default:
            cout << "didnt find piece" << endl;
    }
    if(color == BLACK) board += 6;
    return board;
}

std::vector<Move> Board::getAllMoves(){
    std::vector<Move> newMoves;

    for(int i = 0; i < 12; i++){
        if(boards[i].col != turn) continue;
        // loop through each bitboard
        for(int k = 0; k < 63; k++){
            if((*boards[i].i >> k) & 1){
                // there is a piece here
                U64 mask = generateMoves((U64)1 << k, boards[i].piece, boards[i].col, true);
                // has all moves of this piece
                if(mask == 0) continue;
                Move temp;
                temp.from = *boards[i].i & ((U64)1 << k);
                temp.piece = boards[i].piece;
                temp.color = boards[i].col;
                for(int j = 0; j < 63; j++){
                    if((mask >> j) & 1){
                        temp.to = mask & ((U64)1 << j);
                        newMoves.push_back(temp);
                    }
                }
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
        movePiece(possMoves[i].to);
        moves += Perft(depth - 1);
        unMovePiece();
    }
    
    return moves;
}
