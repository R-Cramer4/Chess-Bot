#include "Game.h"
#include "Resources.h"
#include "SpriteRenderer.h"
#include "Board.h"
#include <cstdio>
#include <cstdlib>
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
    Resources::LoadTexture("textures/king.png", true, "king");
    Resources::LoadTexture("textures/queen.png", true, "queen");
    Resources::LoadTexture("textures/rook.png", true, "rook");
    Resources::LoadTexture("textures/knight.png", true, "knight");
    Resources::LoadTexture("textures/bishop.png", true, "bishop");
    Resources::LoadTexture("textures/pawn.png", true, "pawn");
    Resources::LoadTexture("textures/mask.png", true, "mask");


    turn = WHITE;

    bitboard.generateBitBoards("");
    /*for(int i = 0; i < 2; i++){
        for(int j = 0; j < 6; j++){
            cout << std::bitset<64>(*actBoard.boards[i][j]) << endl;
        }
        cout << endl;
    } // print boards
    */
    

}
void Game::Render(){
    renderer->DrawSprite(Resources::GetTexture("board"), glm::vec2(100.0f, 100.0f), glm::vec2(600.0f, 600.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    U64 temp;
    // color mask
    temp = bitboard.colorMask;
    while(temp != 0){
        int loc = bitboard.findLoc(temp);
        drawPiece(loc % 8, loc / 8, WHITE, "mask", renderer);

        // temp - 1 << loc
        temp = temp - ((U64)1 << (U64)loc);
    }
    
    for(int i = 0; i < 12; i++){ // go through all bitboards
        temp = *bitboard.boards[i].i;
        while(temp != 0){
            int loc = bitboard.findLoc(temp);
            // find location with x and y
            // draw the piece
            drawPiece(loc % 8, loc / 8, bitboard.boards[i].col, bitboard.boards[i].texture, renderer);

            // temp - 1 << loc
            temp = temp - ((U64)1 << (U64)loc);
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

    // invert y
    locy = abs(locy - 7);


    // locx and locy hold the square that was clicked so now update the mask
    U64 loc = ((U64)1 << ((U64)(locy * 8) + (U64)locx));

    // generate potential moves if we click on a piece
    if((bitboard.colorMask & loc) != 0){
        if(*bitboard.selectedPiece.i != loc){
            // we didnt click on the same piece twice
            
            bitboard.movePiece(loc); // move piece
            turn = (turn == WHITE ? BLACK : WHITE);
            *bitboard.selectedPiece.i = 0;
        }
        bitboard.colorMask = 0;
    }else{
        // clicked somewhere not in the mask already

        *bitboard.selectedPiece.i = 0; // sets to be 0 if not already
        for(int i = 0; i < 12; i++){
            if(bitboard.boards[i].col == turn && 
                *bitboard.boards[i].i == (*bitboard.boards[i].i | loc)){

                // update the selected piece
                *bitboard.selectedPiece.i = loc;
                bitboard.selectedPiece.piece = bitboard.boards[i].piece;
                bitboard.selectedPiece.col = bitboard.boards[i].col;

                // If the piece bitboard | loc, this bitboard has been clicked
                bitboard.colorMask = bitboard.generateMoves(loc, bitboard.boards[i].piece, bitboard.boards[i].col, true);
                bitboard.colorMask |= loc;
            }
        }
    }
}

void Game::drawPiece(int x, int y, Color c, string texture, SpriteRenderer *r){
    int locx = ((Width - boardW) / 2) + ((boardW / 8) * x);
    int locy = ((Height - boardH) / 2) + ((boardH / 8) * abs(y - 7));
    // invert y so it draws properly

    r->DrawSprite(Resources::GetTexture(texture), glm::vec2(locx, locy), glm::vec2(boardW / 8, boardH / 8), 0.0f, glm::vec3((float)c));
}
