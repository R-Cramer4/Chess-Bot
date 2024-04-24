#ifndef GAME_H
#define GAME_H

#include "Piece.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Game{
    public:
        void Init();
        void Render();
        void Update(double x, double y);
        
        unsigned int Width = 800;
        unsigned int Height = 800;
        float boardW = 600.0;
        float boardH = 600.0;
        
        Piece *board[8][8];
};


#endif
