#include "Board.h"
#include <iostream>

using namespace std;

void Board::generateBitBoards(string fen){
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    if(fen == "") fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    U64 rank = 7;
    U64 col = 0;

    string positions = fen.substr(0, fen.find(' '));
    string otherInfo = fen.substr(fen.find(' '));
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
    char turn;
    bool whiteCastleKingside = false;
    bool whiteCastleQueenside = false;
    bool blackCastleKingside = false;
    bool blackCastleQueenside = false;
    for(int i = 0; i < otherInfo.size(); i++){
        switch(otherInfo[i]){
            case 'w':
                turn = 'w';
                break;
            case 'b':
                turn = 'b';
                break;
            case 'K':
                whiteCastleKingside = true;
                break;
            case 'Q':
                whiteCastleQueenside = true;
                break;
            case 'k':
                blackCastleKingside = true;
                break;
            case 'q':
                blackCastleQueenside = true;
        }
    }
    // discards enpassant square, half move clock, and fullmove number
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

U64 Board::generateMoves(U64 loc, char piece, Color color){
    // gets all psudo-legal moves for the piece clicked
    // doesnt check for pins yet or check at all
    switch(piece){
        case 'n':
            return getKnightMove(loc, color);
        case 'p':
            return getPawnMove(loc, color);
        case 'b':
            return getBishopMove(loc, color);
        case 'r':
            return getRookMove(loc, color);
        case 'q':
            return getQueenMove(loc, color);
        case 'k':
            return getKingMove(loc, color);
    }
    cout << "Something went wrong, no piece was called" << endl;
    return 0;
}
void Board::movePiece(U64 newSpot){
    if(newSpot & selectedPiece) return;
    // selected piece has the old piece
    // and we have the spot we are moving to

    // need to check if the newSpot is in the other pieces
    for(int i = 0; i < 12; i++){
        if((*boards[i].i & newSpot) != 0){
            *boards[i].i ^= newSpot; // removes that piece
        }
    }
    for(int i = 0; i < 12; i++){
        if((*boards[i].i & selectedPiece) != 0){
            // this is our board
            *boards[i].i ^= selectedPiece;
            *boards[i].i |= newSpot;
        }
    }
}
U64 Board::getRookMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = findLoc(loc);

    mask |= ((U64)0xff << (sq & 56)); // need to shift by rank 0x01-0x0100
    mask |= ((U64)0x0101010101010101 << (sq & 7)); // need to shift by file, 0x01-0x80
    // TODO need to cut off the ray
    return mask;
}
U64 Board::getBishopMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = findLoc(loc);

    int diag = 8*(sq & 7) - (sq & 56);
    int nort = -diag & (diag >> 31);
    int sout = diag & (-diag >> 31);
    mask |= (a1h8Diag >> sout) << nort;

    diag = 56 - 8*(sq & 7) - (sq & 56);
    nort = -diag & (diag >> 31);
    sout = diag & (-diag >> 31);
    mask |= (h1a8AntiDiag >> sout) << nort;
    
    // TODO cut off rays

    return mask;
}
U64 Board::getQueenMove(U64 loc, Color color){
    U64 mask = 0;
    int sq = findLoc(loc);

    mask |= ((U64)0xff << (sq & 56)); // need to shift by rank 0x01-0x0100
    mask |= ((U64)0x0101010101010101 << (sq & 7)); // need to shift by file, 0x01-0x80

    int diag = 8*(sq & 7) - (sq & 56);
    int nort = -diag & (diag >> 31);
    int sout = diag & (-diag >> 31);
    mask |= (a1h8Diag >> sout) << nort;

    diag = 56 - 8*(sq & 7) - (sq & 56);
    nort = -diag & (diag >> 31);
    sout = diag & (-diag >> 31);
    mask |= (h1a8AntiDiag >> sout) << nort;

    // TODO cut off ray

    return mask;
}
U64 Board::getKingMove(U64 loc, Color color){
    // TODO check for if will be in check
    U64 mask = 0;
    mask |= ((loc << 1) & ~aFile) | ((loc >> 1) & ~hFile); // side to side
    mask |= (loc << 8) | (loc >> 8); // up and down
    mask |= ((loc << 7) & ~hFile) | ((loc << 9) & ~aFile); // diags
    mask |= ((loc >> 7) & ~aFile) | ((loc >> 9) & ~hFile); // lower diags

    for(int i = (color ? 0 : 6) ; i < (color ? 6 : 12); i++){
        mask ^= (mask & *boards[i].i); // makes sure this isnt the own colors pieces
    }
    return mask;
}
U64 Board::getPawnMove(U64 loc, Color color){
    U64 mask = 0;

    if(color == WHITE){
        mask = mask | (loc << 8); // not handling overflow
        if(loc & 0x000000000000ff00) mask |= (loc << 16);
    }else{
        mask = mask | (loc >> 8);
        if(loc & 0x00ff000000000000) mask |= (loc >> 16);
    }
    // TODO enpassant, also what happens when it gets to the end
    // TODO can jump over pieces for first move
    U64 attack = 0;
    if(color == WHITE) attack = (loc << 7) | (loc << 9);
    else attack = (loc >> 7) | (loc >> 9);

    U64 opponentPieces = 0;
    for(int i = 0 ; i < 12; i++){
        mask ^= (mask & *boards[i].i); // cant capture forwards
        if(boards[i].col != color) opponentPieces |= *boards[i].i;
    }
    attack = attack & opponentPieces;
    return mask | attack;
}
U64 Board::getKnightMove(U64 loc, Color color){
    // Done (No actual testing though)
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
