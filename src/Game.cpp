#include "Game.h"
#include "Resources.h"
#include "SpriteRenderer.h"
#include "Board.h"

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


    turn = 1;
    // Board Init
    board[0][0] = new Piece(0, 0, 0, 'R', "rook", this); 
    board[1][0] = new Piece(1, 0, 0, 'N', "knight", this);
    board[2][0] = new Piece(2, 0, 0, 'B', "bishop", this);
    board[3][0] = new Piece(3, 0, 0, 'Q', "queen", this);
    board[4][0] = new Piece(4, 0, 0, 'K', "king", this);
    board[5][0] = new Piece(5, 0, 0, 'B', "bishop", this);
    board[6][0] = new Piece(6, 0, 0, 'N', "knight", this);
    board[7][0] = new Piece(7, 0, 0, 'R', "rook", this);

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

    board[0][7] = new Piece(0, 7, 1, 'R', "rook", this);
    board[1][7] = new Piece(1, 7, 1, 'N', "knight", this);
    board[2][7] = new Piece(2, 7, 1, 'B', "bishop", this);
    board[3][7] = new Piece(3, 7, 1, 'Q', "queen", this);
    board[4][7] = new Piece(4, 7, 1, 'K', "king", this);
    board[5][7] = new Piece(5, 7, 1, 'B', "bishop", this);
    board[6][7] = new Piece(6, 7, 1, 'N', "knight", this);
    board[7][7] = new Piece(7, 7, 1, 'R', "rook", this);

    bitboard.generateBitBoards("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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

    /*
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(board[i][j] != nullptr){
                board[i][j]->Draw(renderer);
            }
        }
    }
    */
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
            drawPiece(loc % 8, loc / 8, bitboard.boards[i].c, bitboard.boards[i].s, renderer);

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

    // now check to see if anything ids selected
    /* code for when it was pieces and not bit boards
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
                turn = !turn; // swaps turns

                // here if it is an ai, we need to call its move
                return;
            }
        }
    }
    // this gets clicked now
    if(board[locx][locy] != nullptr){
        if(board[locx][locy]->color == turn) board[locx][locy]->isClicked = true;
        else cout << "not your turn, it is " << turn << "'s turn" << endl;
    }
    */

    // locx and locy hold the square that was clicked so now update the mask
    bitboard.colorMask ^= ((U64)1 << ((U64)(locy * 8) + (U64)locx));
}

void Game::drawPiece(int x, int y, Color c, string texture, SpriteRenderer *r){
    int locx = ((Width - boardW) / 2) + ((boardW / 8) * x);
    int locy = ((Height - boardH) / 2) + ((boardH / 8) * y);

    r->DrawSprite(Resources::GetTexture(texture), glm::vec2(locx, locy), glm::vec2(boardW / 8, boardH / 8), 0.0f, glm::vec3((float)c));
}
