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
void Board::printLoc(U64 x){
    printf("0x%016llx\n", x);
}

U64 Board::generateMoves(U64 loc, char piece, Color color){
    U64 mask = 0; // this function will return all valid moves
    
    U64 notAFile = ~0x0101010101010101;
    U64 notABFile = ~0x0303030303030303;
    U64 notHFile = ~0x8080808080808080;
    U64 notGHFile = ~0xC0C0C0C0C0C0C0C0;

    printLoc(loc);

    switch(piece){
        case 'n': // should work, knights
            mask = mask | ((loc << 17) & notAFile);
            mask = mask | ((loc << 10) & notABFile);
            mask = mask | ((loc >> 6) & notABFile);
            mask = mask | ((loc >> 15) & notAFile);

            mask = mask | ((loc >> 17) & notHFile);
            mask = mask | ((loc >> 10) & notGHFile);
            mask = mask | ((loc << 6) & notGHFile);
            mask = mask | ((loc << 15) & notHFile);
            for(int i = (color ? 0 : 6) ; i < (color ? 6 : 12); i++){
                mask ^= (mask & *boards[i].i); // makes sure this isnt the own colors pieces
            }
            break;
        case 'p':
            if(color == WHITE){
                mask = mask | (loc >> 8); // not handling overflow
                if(loc & 0x00ff000000000000) mask = mask | (loc >> 16);
            }else{
                mask = mask | (loc << 8);
                if(loc & 0x00ff00) mask = mask | (loc << 16);
            }
            // TODO enpassant, capture, also what happens when it gets to the end
            // TODO also cant capture forwards
            for(int i = (color ? 0 : 6) ; i < (color ? 6 : 12); i++){
                mask ^= (mask & *boards[i].i); // makes sure this isnt the own colors pieces
            }
            break;
        case 'b':

            break;
        case 'r':
            mask |= (0xff << (loc & 56)); // need to shift by rank 0x01-0x0100
            mask |= (0x0101010101010101 << (loc & 7)); // need to shift by file, 0x01-0x80
            printLoc(loc & 56);
            printLoc(loc & 7);
            // doesnt work properly
            // need to cut off the ray
            break;
        case 'q':

            break;
        case 'k':

            break;
    }




    return mask;
}
void Board::movePiece(U64 newSpot){
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
