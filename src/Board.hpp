#ifndef BOARD_H
#define BOARD_H

#include <stack>
#include <string>
#include "Masks.hpp"


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
struct Move{
    U64 from;
    U64 to;
    char special; // 0 - 15
    char piece;
    Color color;
    // 0 = normal moves
    // 1 = double pawn psuh
    // 2 = king castle
    // 3 = queen castle
    // 4 = captures
    // 5 = enpassant capture
    // 8 = knight promo
    // 9 = bishop promo
    // 10 = rook promo
    // 11 = queen promo
    // 12 - 15 = promo with capture
    char castleRight;
    // K = taken away white kingside castle
    // Q = taken away white queenside castle
    // k = taken away black kingside castle
    // q = taken away black queenside castle
    // W = taken away both white castles
    // B = taken away both black castles
    bool operator==(Move m){
        return m.to == this->to &&
                m.from == this->from &&
                m.piece == this->piece &&
                m.color == this->color && 
                m.special == this->special && 
                m.castleRight == this->castleRight;
    }
};

class Board{
    public:
        int gameOver = 0; // 0 means still playing
        // 1 = white wins by checkmate
        // 2 = black wins by checkmate
        // 3 = stalemate
        // 4 = insufficient material
        // 5 = 50 move rule
        // 6 = stalemate by repitition

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

        U64 blackPieces = 0;
        U64 whitePieces = 0;

        U64 colorMask = 0;
        U64 debugMask = kingMask;

        U64 enpassantLoc = 0;
        U64 pawnPromo = 0; // becomes the loc if a pawn is waiting on promotion

        bool whiteCastleKing = 0;
        bool whiteCastleQueen = 0;
        bool blackCastleKing = 0;
        bool blackCastleQueen = 0;

        int halfMoves = 0;
        int fullMoves = 0;
        std::stack<Move> moves;
        std::stack<std::pair<char, Color>> captures;

        Color turn = WHITE;

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
        
        Board(){}
        Board(Board &ref);

        Color generateBitBoards(std::string fen); // takes a string in with fen notation to setup the bitboards, init string usually
        void reset(std::string fen); // resets everything with new fen
        void printLoc(U64 x);

        int isGameOver();
        void printBoardState();
        std::string getMove(Move m);

        U64 generateMoves(U64 loc, char type, Color color, bool top);
        std::vector<Move> getAllMoves();
        U64 Perft(int depth);

        void movePiece(U64 from, Color color, char piece, U64 newSpot);
        void promotePawn(U64 loc, Color color, char to);
        void unMovePiece();

        int getLSLoc(U64 mask);

    private:
        U64 getRookMove(U64 loc, Color color);
        U64 getBishopMove(U64 loc, Color color);
        U64 getQueenMove(U64 loc, Color color);
        U64 getKingMove(U64 loc, Color color);
        U64 getPawnMove(U64 loc, Color color);
        U64 getKnightMove(U64 loc, Color color);

        U64 getActualRay(U64 loc, U64 ray, Color color);
        bool isKingInCheck(Color c);

        int getBoard(char piece, Color color);
        std::string boardToLoc(U64 board);
};

#endif
