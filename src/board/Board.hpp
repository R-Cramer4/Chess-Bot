#ifndef BOARD_H
#define BOARD_H

#include <stack>
#include <string>
#include <vector>
#include "Masks.hpp"

struct quad{
    U64* i;
    Color col;
    std::string texture;
    char piece;
};
struct Move{
    U64 from = 0;
    U64 to = 0;
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
    // 0b1111 = taken away all castles = 15
    // 0b0000 = taken away no castles = 0
    // 0b1100 = white castles = 12
    // 0b0011 = black castles = 
    // 0b1010 = kingside castles
    // 0b0101 = queenside castles
    U64 enpassant = 0; 
    // the enpassant square before anything was mvoed

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
    U64 debugMask = 0;

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

    Board() = default;
    //Board(Board &ref);

    Color generateBitBoards(std::string fen); // takes a string in with fen notation to setup the bitboards, init string usually
    void reset(std::string fen); // resets everything with new fen
    void printLoc(U64 x);

    int isGameOver(bool act);
    void printBoardState();
    std::string getMove(Move m);

    // moves needs to be of size 256
    U64 generateMoves(U64 loc, char type, Color color, bool top, Move *moves);
    std::vector<Move> getAllMoves();
    U64 Perft(int depth);

    Move movePiece(U64 from, Color color, char piece, U64 newSpot);
    void movePiece(Move move);
    void promotePawn(U64 loc, Color color, char to);
    void unMovePiece();

    int getLSLoc(U64 mask);
    int isKingInCheck(Color c);

    float evalBoard(Color c);

    U64 flipVertical(U64 x);

private:
    U64 getRookMove(U64 loc, Color color);
    U64 getBishopMove(U64 loc, Color color);
    U64 getQueenMove(U64 loc, Color color);
    U64 getKingMove(U64 loc, Color color);
    U64 getPawnMove(U64 loc, Color color);
    U64 getKnightMove(U64 loc, Color color);

    U64 getActualRay(U64 loc, U64 ray, Color color);

    char handleCapture(U64 to, Color color, bool handleCastle);

    U64 *getBoard(char piece, Color color);
    std::string boardToLoc(U64 board);

    int countBits(U64 i);
    U64 getWhitePieces();
    U64 getBlackPieces();
    void getAllMovesFromBoard(U64 board, char piece, Color color, std::vector<Move> &newMoves);
};

#endif
