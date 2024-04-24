#include "Game.h"
#include "Resources.h"
#include "SpriteRenderer.h"
#include <iostream>

SpriteRenderer *renderer; 

void Game::Init(){
    // load shaders
    Resources::LoadShader("src/vertex.vert", "src/fragment.frag", nullptr, "sprite");
    // shader config
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    Resources::GetShader("sprite").Use().SetInteger("image", 0);
    Resources::GetShader("sprite").SetMatrix4("projection", projection);

    renderer = new SpriteRenderer(Resources::GetShader("sprite"));
    Resources::LoadTexture("textures/board.png", false, "board");
    Resources::LoadTexture("textures/pawn.png", true, "pawn");

    // Board Init
    board[0][0] = new Piece(0, 0, 0, 'R', "pawn", this); 
    board[1][0] = new Piece(1, 0, 0, 'N', "pawn", this);
    board[2][0] = new Piece(2, 0, 0, 'B', "pawn", this);
    board[3][0] = new Piece(3, 0, 0, 'Q', "pawn", this);
    board[4][0] = new Piece(4, 0, 0, 'K', "pawn", this);
    board[5][0] = new Piece(5, 0, 0, 'B', "pawn", this);
    board[6][0] = new Piece(6, 0, 0, 'N', "pawn", this);
    board[7][0] = new Piece(7, 0, 0, 'R', "pawn", this);

    board[0][1] = new Piece(0, 1, 0, 'P', "pawn", this);
    board[1][1] = new Piece(1, 1, 0, 'P', "pawn", this);
    board[2][1] = new Piece(2, 1, 0, 'P', "pawn", this);
    board[3][1] = new Piece(3, 1, 0, 'P', "pawn", this);
    board[4][1] = new Piece(4, 1, 0, 'P', "pawn", this);
    board[5][1] = new Piece(5, 1, 0, 'P', "pawn", this);
    board[6][1] = new Piece(6, 1, 0, 'P', "pawn", this);
    board[7][1] = new Piece(7, 1, 0, 'P', "pawn", this);

    board[0][6] = new Piece(0, 6, 1, 'P', "pawn", this);
    board[1][6] = new Piece(1, 6, 1, 'P', "pawn", this);
    board[2][6] = new Piece(2, 6, 1, 'P', "pawn", this);
    board[3][6] = new Piece(3, 6, 1, 'P', "pawn", this);
    board[4][6] = new Piece(4, 6, 1, 'P', "pawn", this);
    board[5][6] = new Piece(5, 6, 1, 'P', "pawn", this);
    board[6][6] = new Piece(6, 6, 1, 'P', "pawn", this);
    board[7][6] = new Piece(7, 6, 1, 'P', "pawn", this);

    board[0][7] = new Piece(0, 7, 1, 'R', "pawn", this);
    board[1][7] = new Piece(1, 7, 1, 'N', "pawn", this);
    board[2][7] = new Piece(2, 7, 1, 'B', "pawn", this);
    board[3][7] = new Piece(3, 7, 1, 'Q', "pawn", this);
    board[4][7] = new Piece(4, 7, 1, 'K', "pawn", this);
    board[5][7] = new Piece(5, 7, 1, 'B', "pawn", this);
    board[6][7] = new Piece(6, 7, 1, 'N', "pawn", this);
    board[7][7] = new Piece(7, 7, 1, 'R', "pawn", this);

}
void Game::Render(){
    renderer->DrawSprite(Resources::GetTexture("board"), glm::vec2(100.0f, 100.0f), glm::vec2(600.0f, 600.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] != nullptr){
                board[i][j]->Draw(renderer);
            }
        }
    }
}
void Game::Update(double x, double y){
    // handle mouse clicks
    int locx, locy;
    locx = x - ((Width - boardW) / 2); // goes to edge of board
    locx = locx / (boardW / 8); // now goes to pos
    locy = y - ((Height - boardH) / 2); // goes to edge of board
    locy = locy / (boardH / 8); // now goes to pos

    // now check to see if anything ids selected
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] != nullptr && board[i][j]->isClicked){
                board[i][j]->isClicked = false;
                // TODO check valid move and check for capture

                // swaps pieces
                Piece *temp = board[i][j];
                board[i][j] = board[locx][locy];
                if(board[i][j] != nullptr) board[i][j]->UpdatePos(i, j, this);
                board[locx][locy] = temp;
                if(board[locx][locy] != nullptr) board[locx][locy]->UpdatePos(locx, locy, this);
                return;
            }
        }
    }
    // this gets clicked now
    if(board[locx][locy] != nullptr) board[locx][locy]->isClicked = true;
}
