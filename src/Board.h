#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <string>

typedef uint64_t U64;
enum Color {BLACK, WHITE};

class triple{
    public:
        triple(U64* i, Color c, std::string s){
            this->i = i;
            this->c = c;
            this->s = s;
        }
        U64* i;
        Color c;
        std::string s;
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

        triple boards[12] = {
            {&whitePawns, WHITE, "pawn"},
            {&whiteKnights, WHITE, "knight"},
            {&whiteBishops, WHITE, "bishop"},
            {&whiteRooks, WHITE, "rook"},
            {&whiteQueens, WHITE, "queen"},
            {&whiteKing, WHITE, "king"},
            {&blackPawns, BLACK, "pawn"},
            {&blackKnights, BLACK, "knight"},
            {&blackBishops, BLACK, "bishop"},
            {&blackRooks, BLACK, "rook"},
            {&blackQueens, BLACK, "queen"},
            {&blackKing, BLACK, "king"}
        };


        void generateBitBoards(std::string fen); // takes a string in with fen notation to setup the bitboards, init string usually
        int findLoc(U64 x);
};

#endif
