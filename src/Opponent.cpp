#include "Opponent.hpp"
#include <climits>
#include <cstdlib>
#include <iostream>


int Opponent::takeTurn(Board &b){

    Move m;
    // in the future when have different type of move algorithms
    switch(this->t){
        case RAND:
            m = FindRandomMove(b);
            break;
        case AI:
            m = FindRandomMove(b);
            break;
        case MINMAX:
            m = FindRandomMove(b);
            break;
        case ALGO:
            m = FindBestMove(b);
            break;
    }

    b.movePiece(m);
    return 0;
}

Move Opponent::FindBestMove(Board &b){
    // goes to a depth of 1
    Move bestMove;
    float bestScore = INT_MIN;

    float currentEval = b.evalBoard(c);

    Move moves[256];

    // go through all moves that opp can make
    int start = c == WHITE ? 0 : 6;
    for(int i = start + 5; i >= start; i--){ // go backwards so least valuable pieces capture
        U64 curMask = *b.boards[i].i;
        while(curMask != 0){
            // go through every piece
            U64 loc = (curMask & -curMask);
            curMask ^= loc;

            U64 len = b.generateMoves(loc, b.boards[i].piece, b.boards[i].col, true, moves);
            // eval every move for c
            for(int k = 0; k < len; k++){
                b.movePiece(moves[k]);
                float newScore = b.evalBoard(c) - currentEval;
                b.unMovePiece();
                if(newScore >= bestScore){
                    bestScore = newScore;
                    bestMove = moves[k];
                }
            }
        }
    }
    if(bestScore == 0) return FindRandomMove(b); // just pick something random if no captures

    return bestMove;
}

Move Opponent::FindRandomMove(Board &b){
    auto moves = b.getAllMoves();
    int i = rand() % moves.size();

    return moves[i];
}
