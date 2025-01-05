#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>

typedef uint64_t U64;
enum Color {BLACK, WHITE};

class quad{
    public:
        quad(U64* i, Color c, std::string s, char p){
            this->i = i;
            this->col = c;
            this->texture = s;
            this->piece = p;
        }
        U64* i;
        Color col;
        std::string texture;
        char piece;
};

class Board{
    public:
        // using a binary representation where the least significant bit is A1
        // the most significant bit is H8
        // so the first 8 bits are A1 - A8
        U64 whitePawns = 0;
        U64 whiteKnights = 0;
        U64 whiteBishops = 0;
        U64 whiteRooks = 0;
        U64 whiteQueens = 0;
        U64 whiteKing = 0;

        U64 blackPawns = 0;
        U64 blackKnights = 0;
        U64 blackBishops = 0;
        U64 blackRooks = 0;
        U64 blackQueens = 0;
        U64 blackKing = 0;

        U64 colorMask = 0;
        U64 selectedPiece = 0;

        bool whiteCastle = 1;
        bool blackCastle = 1;

        quad boards[12] = {
            {&whitePawns, WHITE, "pawn", 'p'},
            {&whiteKnights, WHITE, "knight", 'n'},
            {&whiteBishops, WHITE, "bishop", 'b'},
            {&whiteRooks, WHITE, "rook", 'r'},
            {&whiteQueens, WHITE, "queen", 'q'},
            {&whiteKing, WHITE, "king", 'k'},
            {&blackPawns, BLACK, "pawn", 'p'},
            {&blackKnights, BLACK, "knight", 'n'},
            {&blackBishops, BLACK, "bishop", 'b'},
            {&blackRooks, BLACK, "rook", 'r'},
            {&blackQueens, BLACK, "queen", 'q'},
            {&blackKing, BLACK, "king", 'k'}
        };


        void generateBitBoards(std::string fen); // takes a string in with fen notation to setup the bitboards, init string usually
        int findLoc(U64 x);
        void printLoc(U64 x);

        U64 generateMoves(U64 loc, char type, Color color);
        void movePiece(U64 newSpot);
};

#endif
