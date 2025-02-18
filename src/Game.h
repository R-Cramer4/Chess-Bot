#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "SpriteRenderer.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Game{
    public:
        void Init(string fen = "", int num = 0);
        void Render();
        bool Update(double x, double y); // mouse has clicked this pos
        void Clear();
        
        unsigned int Width = 800;
        unsigned int Height = 800;
        float boardW = 600.0;
        float boardH = 600.0;

        Board bitboard = Board();
        void drawPiece(int x, int y, Color c, string texture, SpriteRenderer *r);

        Color turn;
        U64 lastClicked = 0;
        quad selectedPiece = {&lastClicked, NONE, "", '0'};

        void pawnPromo(double x, double y);

        Color checkmate = NONE;
};


#endif
