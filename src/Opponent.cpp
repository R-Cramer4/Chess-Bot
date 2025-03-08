#include "Opponent.hpp"
#include <cstdlib>


int Opponent::takeTurn(Board &b){
    auto moves = b.getAllMoves();

    Move m = FindRandomMove(b, moves);

    b.movePiece(m);
    return 0;
}


Move Opponent::FindRandomMove(Board &b, vector<Move> &moves){
    int i = rand() % moves.size();

    return moves[i];
}
