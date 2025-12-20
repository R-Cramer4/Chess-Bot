#include "Game.hpp"
#include "Board.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>

Game::Game(string fen, int num)
    : opp(bitboard, BLACK)
{
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
    opp.setBoard(bitboard);
}
bool Game::handleClick(int x) {
    if(bitboard.pawnPromo){
        printf("need to handle pawn promo\n");
        // pawnPromo(x, y);
        return true;
    }
    if(bitboard.debugMask != 0) bitboard.debugMask = 0;
    //cout << "WHITE: " << bitboard.isKingInCheck(WHITE) << endl;
    //cout << "BLACK: " << bitboard.isKingInCheck(BLACK) << endl;
    //cout << bitboard.whiteCastleKing << bitboard.whiteCastleQueen << endl;
    //cout << bitboard.blackCastleKing << bitboard.blackCastleQueen << endl;


    U64 loc = (U64)1 << x;
    // mirror vertically after wrong rust -> c++ conversion
    loc = bitboard.flipVertical(loc);

    Move m;
    bool moveMade = false;

    // generate potential moves if we click on a piece
    if ((bitboard.colorMask & loc) != 0){
        if(*selectedPiece.i != loc && *selectedPiece.i != 0){
            // we didnt click on the same piece twice
            
            for(int i = 0; i < 256; i++){
                if(potentialMoves[i].from == 0) continue;
                if(potentialMoves[i].to == loc){
                    m = potentialMoves[i];
                    moveMade = true;
                }
            }
            //bitboard.movePiece(*selectedPiece.i, selectedPiece.col, selectedPiece.piece, loc); // move piece
            bitboard.movePiece(m);
            *selectedPiece.i = 0;
        }
        bitboard.colorMask = 0;
    } else {
        bitboard.colorMask = 0;
        // clicked somewhere not in the mask already

        *selectedPiece.i = 0; // sets to be 0 if not already
        auto findMoves = [this, loc](U64 board, char piece, Color color) {
            if (color == bitboard.turn && 
                board == (board | loc)){

                // update the selected piece
                *selectedPiece.i = loc;
                selectedPiece.piece = piece;
                selectedPiece.col = color;

                // If the piece bitboard | loc, this bitboard has been clicked
                std::memset(potentialMoves, 0, 256); // nulls all old moves
                U64 len = bitboard.generateMoves(loc, piece, color, true, potentialMoves);
                for(int i = 0; i < len; i++){
                    if(potentialMoves[i].from) bitboard.colorMask |= potentialMoves[i].to;
                }
                bitboard.colorMask |= loc;
            }
        };

        findMoves(bitboard.whitePawns, 'p', WHITE);
        findMoves(bitboard.whiteKnights, 'n', WHITE);
        findMoves(bitboard.whiteBishops, 'b', WHITE);
        findMoves(bitboard.whiteRooks, 'r', WHITE);
        findMoves(bitboard.whiteQueens, 'q', WHITE);
        findMoves(bitboard.whiteKing, 'k', WHITE);
        findMoves(bitboard.blackPawns, 'p', BLACK);
        findMoves(bitboard.blackKnights, 'n', BLACK);
        findMoves(bitboard.blackBishops, 'b', BLACK);
        findMoves(bitboard.blackRooks, 'r', BLACK);
        findMoves(bitboard.blackQueens, 'q', BLACK);
        findMoves(bitboard.blackKing, 'k', BLACK);

        if(*selectedPiece.i == 0) bitboard.colorMask = loc;
    }

    // opponent move
    if (moveMade && bitboard.turn == opp.color) {
        // TODO handle the case its currently a pawn promo screen
        opp.moveTaken(m); 
        bitboard.movePiece(opp.takeTurn());
    }

    if(bitboard.isGameOver(true) != 0) return false;
    if (bitboard.whiteKing == 0) {
        cout << "Black wins" << endl;
        return false;
    } else if (bitboard.blackKing == 0) {
        cout << "White wins" << endl;
        return false;
    }
    return true;
}

void Game::pawnPromo(double x, double y){
    // TODO
    /*
    // create pawn promo screen and handle clicks
    int locx, locy;
    locx = x - ((Width - boardW) / 2); // goes to edge of board
    locx = locx / (boardW / 2); // now goes to pos
    locy = y - ((Height - boardH) / 2); // goes to edge of board
    locy = locy / (boardH / 2); // now goes to pos
    
    // not clamped to exactly on the img, so if you click off it it still gives you a pos in the quadrant you want

    Color c = WHITE;
    if(bitboard.turn == WHITE) c = BLACK;

    char piece;
    if(locx == 0 && locy == 0) piece = 'b';
    else if(locx == 0 && locy == 1) piece = 'q';
    else if(locy == 0) piece = 'n';
    else piece = 'r';

    // here i think we just update the bitboards instead of making a move 
    // because the pawn is already there
    // so its not a pawn it is a queen that always goes there
    int loc = 0;
    if(bitboard.turn == WHITE) loc = 6;
    *bitboard.boards[loc + 4].i ^= bitboard.pawnPromo;
    // need to update the last move special
    Move m = bitboard.moves.top();
    bitboard.moves.pop();
    char special = m.special & 4; // isolates if capture

    switch (piece) {
        case 'n':
            *bitboard.boards[loc + 1].i ^= bitboard.pawnPromo;
            special += 8;
            break;
        case 'b':
            *bitboard.boards[loc + 2].i ^= bitboard.pawnPromo;
            special += 9;
            break;
        case 'r':
            *bitboard.boards[loc + 3].i ^= bitboard.pawnPromo;
            special += 10;
            break;
        case 'q':
            *bitboard.boards[loc + 4].i ^= bitboard.pawnPromo;
            special += 11;
            break;
    }
    m.special = special;
    bitboard.moves.push(m);

    bitboard.pawnPromo = 0;

    // now make opp move
    if(bitboard.turn == opp.c) opp.takeTurn(bitboard);
    */

}
