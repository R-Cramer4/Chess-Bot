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

const U64 aFile =         0x0101010101010101;
const U64 abFile =        0x0303030303030303;
const U64 hFile =         0x8080808080808080;
const U64 ghFile =        0xC0C0C0C0C0C0C0C0;
const U64 rank1 =         0x00000000000000FF;
const U64 rank8 =         0xFF00000000000000;
const U64 a1h8Diag =      0x8040201008040201;
const U64 h1a8AntiDiag =  0x0102040810204080;
const U64 lightSquares =  0x55AA55AA55AA55AA;
const U64 darkSquares =   0xAA55AA55AA55AA55;

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
        U64 selecPieceLoc = 0;
        quad selectedPiece = {&selecPieceLoc, WHITE, "", 'p'};

        U64 enpassantBlack = 0;
        U64 enpassantWhite = 0;

        bool whiteCastleKing = 1;
        bool whiteCastleQueen = 1;
        bool blackCastleKing = 1;
        bool blackCastleQueen = 1;

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
    private:
        U64 getRookMove(U64 loc, Color color);
        U64 getBishopMove(U64 loc, Color color);
        U64 getQueenMove(U64 loc, Color color);
        U64 getKingMove(U64 loc, Color color);
        U64 getPawnMove(U64 loc, Color color);
        U64 getKnightMove(U64 loc, Color color);

        U64 getActualRay(U64 loc, U64 ray, Color color);
};

#endif
