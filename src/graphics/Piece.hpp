#ifndef PIECE_H
#define PIECE_H

#include "SpriteRenderer.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>

using namespace std;

class Game;

class Piece {
    public:
        Piece(int x, int y, bool color, char type, string texture, Game *g);
        void UpdatePos(int x, int y, Game *g);
        void Draw(SpriteRenderer *r);
        glm::vec2 pos; // where it is on the board, goes from 0,0 - 8,8
        bool color; // 1 is white, 0 is black
        char type; // K[ing], Q[ueen], k[N]ight, R[ook], B[ishop], P[awn]
        string texture;
        bool isClicked = false;

    private:
        glm::vec2 localPos;
        glm::vec2 size;


};

#endif
