#include "Opponent.hpp"
#include <cstdlib>


int Opponent::takeTurn(Board &b){
    auto moves = b.getAllMoves();

    Move m;
    // in the future when have different type of move algorithms
    switch(this->t){
        case RAND:
            m = FindRandomMove(b, moves);
            break;
        case AI:
            m = FindRandomMove(b, moves);
            break;
        case MINMAX:
            m = FindRandomMove(b, moves);
            break;
    }

    b.movePiece(m);
    return 0;
}


Move Opponent::FindRandomMove(Board &b, vector<Move> &moves){
    int i = rand() % moves.size();

    return moves[i];
}
