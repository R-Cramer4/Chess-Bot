#ifndef GAME_H
#define GAME_H

#include "Board.hpp"
#include "Opponent.hpp"

class Game{
public:
    Game(string fen = "", int num = 0);

    // returns true if the game is still going, false otherwise
    bool handleClick(int x);

    Board bitboard = Board();
    Opponent opp;

    Color turn;
    U64 lastClicked = 0;
    quad selectedPiece = {&lastClicked, NONE, "", '0'};
    Move potentialMoves[256];

    void pawnPromo(double x, double y);

    Color checkmate = NONE;
};


#endif
