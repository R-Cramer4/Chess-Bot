#ifndef GAME_H
#define GAME_H

#include "Board.hpp"
#include "Opponent.hpp"

class Game{
    public:
        void Init(string fen = "", int num = 0);
        bool Update(double x, double y); // mouse has clicked this pos
        void Clear();
        
        unsigned int Width = 800;
        unsigned int Height = 800;
        float boardW = 600.0;
        float boardH = 600.0;

        Board bitboard = Board();
        Opponent opp = Opponent(BLACK);

        Color turn;
        U64 lastClicked = 0;
        quad selectedPiece = {&lastClicked, NONE, "", '0'};
        Move potentialMoves[256];

        void pawnPromo(double x, double y);

        Color checkmate = NONE;
};


#endif
