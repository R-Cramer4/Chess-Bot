#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "SpriteRenderer.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Game{
    public:
        void Init();
        void Render();
        void Update(double x, double y); // mouse has clicked this pos
        
        unsigned int Width = 800;
        unsigned int Height = 800;
        float boardW = 600.0;
        float boardH = 600.0;

        Board bitboard;
        void drawPiece(int x, int y, Color c, string texture, SpriteRenderer *r);

        Color turn;
};


#endif
