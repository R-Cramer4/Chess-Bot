#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <stack>
#include <string>
#include <vector>

typedef uint64_t U64;
enum Color {BLACK, WHITE, NONE};

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

        U64 colorMask = 0;
        U64 debugMask = 0;

        U64 enpassantLoc = 0;

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
        

        Color generateBitBoards(std::string fen); // takes a string in with fen notation to setup the bitboards, init string usually
        void reset(std::string fen); // resets everything with new fen
        int getLSLoc(U64 mask);
        void printLoc(U64 x);

        U64 generateMoves(U64 loc, char type, Color color, bool top);
        std::vector<Move> getAllMoves();
        void movePiece(U64 from, Color color, char piece, U64 newSpot);
        void unMovePiece();
        int isGameOver();
        Board(){}
        Board(Board &ref);
        U64 Perft(int depth);
        void printBoardState();
        std::string getMove(Move m);

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
