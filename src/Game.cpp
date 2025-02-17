#include "Game.h"
#include "Resources.h"
#include "SpriteRenderer.h"
#include "Board.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

SpriteRenderer *renderer; 

void Game::Init(string fen, int num){
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
    Resources::LoadTexture("textures/pawn_promo.png", true, "pawnPromo");



    if(fen == "perft") turn = bitboard.generateBitBoards("");
    else turn = bitboard.generateBitBoards(fen);

    if(fen == "perft"){
        for(int i = 1; i <= num; i++){
            U64 perft = bitboard.Perft(i);
            cout << i << " : " << perft << endl;
        }
        turn = NONE;
        //cout << "Generate moves : " << bitboard.generateMovesCalls << endl;
        //cout << "Get all moves : " << bitboard.getAllMovesCalls << endl;
        //cout << "Move Piece : " << bitboard.movePieceCalls << endl;
        //cout << "Unmove Piece : " << bitboard.unmovePieceCalls << endl;
        //cout << "Is king in check calls : " << bitboard.isKingInCheckCalls << endl;
        //cout << "get board calls : " << bitboard.getBoardCalls << endl;
    }
}
void Game::Clear(){
    Resources::Clear();
    delete renderer;
}
void Game::Render(){
    renderer->DrawSprite(Resources::GetTexture("board"), glm::vec2(100.0f, 100.0f), glm::vec2(600.0f, 600.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    U64 temp;
    // color mask
    temp = bitboard.colorMask;
    while(temp != 0){
        int loc = bitboard.getLSLoc(temp);
        drawPiece(loc % 8, loc / 8, WHITE, "mask", renderer);

        // temp - 1 << loc
        temp = temp - ((U64)1 << (U64)loc);
    }
    temp = bitboard.debugMask;
    while(temp != 0){
        int loc = bitboard.getLSLoc(temp);
        drawPiece(loc % 8, loc / 8, WHITE, "mask", renderer);

        // temp - 1 << loc
        temp = temp - ((U64)1 << (U64)loc);
    }
    
    for(int i = 0; i < 12; i++){ // go through all bitboards
        temp = *bitboard.boards[i].i;
        while(temp != 0){
            int loc = bitboard.getLSLoc(temp);
            // find location with x and y
            // draw the piece
            drawPiece(loc % 8, loc / 8, bitboard.boards[i].col, bitboard.boards[i].texture, renderer);

            // temp - 1 << loc
            temp = temp - ((U64)1 << (U64)loc);
        }
    }

    if(isPawnPromo){
        renderer->DrawSprite(Resources::GetTexture("pawnPromo"), 
                            glm::vec2(boardW / 2, boardH / 2), glm::vec2(200.0f, 200.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    }
}
bool Game::Update(double x, double y){
    if(isPawnPromo){
        pawnPromo(x, y);
        return true;
    }
    if(bitboard.debugMask != 0) bitboard.debugMask = 0;
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
        if(*selectedPiece.i != loc && *selectedPiece.i != 0){
            // we didnt click on the same piece twice
            
            bitboard.movePiece(*selectedPiece.i, selectedPiece.col, selectedPiece.piece, loc); // move piece


            Move lastMove = bitboard.moves.top(); 
            // has to have something because we just called move
            if(lastMove.piece == 'p'){
                if(lastMove.color == WHITE){
                    if(loc & 0xff00000000000000){
                        // was a promotion
                        isPawnPromo = loc;
                    }
                }else if(lastMove.color == BLACK){
                    if(loc & (U64)0xff){
                        isPawnPromo = loc;
                    }
                }
            }

            *selectedPiece.i = 0;
        }
        bitboard.colorMask = 0;
    }else{
        // clicked somewhere not in the mask already

        *selectedPiece.i = 0; // sets to be 0 if not already
        for(int i = 0; i < 12; i++){
            if(bitboard.boards[i].col == bitboard.turn && 
                *bitboard.boards[i].i == (*bitboard.boards[i].i | loc)){

                // update the selected piece
                *selectedPiece.i = loc;
                selectedPiece.piece = bitboard.boards[i].piece;
                selectedPiece.col = bitboard.boards[i].col;

                // If the piece bitboard | loc, this bitboard has been clicked
                bitboard.colorMask = bitboard.generateMoves(loc, bitboard.boards[i].piece, bitboard.boards[i].col, true);
                bitboard.colorMask |= loc;
            }
        }
        if(*selectedPiece.i == 0) bitboard.colorMask = loc;
    }
    if(bitboard.isGameOver() != 0) return false;
    if(*bitboard.boards[5].i == 0){
        cout << "Black wins" << endl;
        return false;
    }else if(*bitboard.boards[11].i == 0){
        cout << "White wins" << endl;
        return false;
    }
    return true;
}

void Game::drawPiece(int x, int y, Color c, string texture, SpriteRenderer *r){
    int locx = ((Width - boardW) / 2) + ((boardW / 8) * x);
    int locy = ((Height - boardH) / 2) + ((boardH / 8) * abs(y - 7));
    // invert y so it draws properly

    r->DrawSprite(Resources::GetTexture(texture), glm::vec2(locx, locy), glm::vec2(boardW / 8, boardH / 8), 0.0f, glm::vec3((float)c));
}
void Game::pawnPromo(double x, double y){
    // create pawn promo screen and handle clicks
    int locx, locy;
    locx = x - ((Width - boardW) / 2); // goes to edge of board
    locx = locx / (boardW / 2); // now goes to pos
    locy = y - ((Height - boardH) / 2); // goes to edge of board
    locy = locy / (boardH / 2); // now goes to pos
    
    // not clamped to exactly on the img, so if you click off it it still gives you a pos in the quadrant you want

    // the piece we want is in isPawnPromo, the color is ~Turn

    int board = 0;
    if(bitboard.turn == WHITE) board = 6;

    // knight += 1
    // bishop += 2
    // rook += 3
    // queen += 4
    int add = 0;
    if(locx == 0 && locy == 0) add = 2;
    else if(locx == 0 && locy == 1) add = 4;
    else if(locy == 0) add = 1;
    else add = 3;

    *bitboard.boards[board].i ^= isPawnPromo; // get rid of the pawn
    *bitboard.boards[board + add].i |= isPawnPromo; // add piece

    // add to last move
    auto move = bitboard.moves.top();
    bitboard.moves.pop();

    // if captured a piece, ranges from 12 - 15
    // otherwise ranges from 8-11
    move.special += 7 + add;
    bitboard.moves.push(move);
    
    isPawnPromo = 0;
}
