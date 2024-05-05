#include "Board.h"

using namespace std;

void Board::generateBitBoards(string fen){
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    U64 rank = 0;
    U64 col = 0;

    string positions = fen.substr(0, fen.find(' '));
    string otherInfo = fen.substr(fen.find(' '));
    for(int i = 0; i < positions.size(); i++){
        U64 pos = ((U64)1 << ((rank * (U64)8) + col));
        switch(fen[i]){
            case '/':
                rank++;
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
    }

    return loc;
}
