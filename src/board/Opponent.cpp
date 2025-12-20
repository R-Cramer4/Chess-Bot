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
    auto evalMoves = [this, currentEval](Board &b, U64 board, char piece, Color col, Move *moves, float &bestScore, Move &bestMove) {
        U64 curMask = board;
        while(curMask != 0){
            // go through every piece
            U64 loc = (curMask & -curMask);
            curMask ^= loc;

            U64 len = b.generateMoves(loc, piece, col, true, moves);
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
    };
    if (c == WHITE) {
        // go backwards so least valuable pieces capture
        evalMoves(b, b.whiteKing, 'k', WHITE, moves, bestScore, bestMove);
        evalMoves(b, b.whiteQueens, 'q', WHITE, moves, bestScore, bestMove);
        evalMoves(b, b.whiteRooks, 'r', WHITE, moves, bestScore, bestMove);
        evalMoves(b, b.whiteBishops, 'b', WHITE, moves, bestScore, bestMove);
        evalMoves(b, b.whiteKnights, 'n', WHITE, moves, bestScore, bestMove);
        evalMoves(b, b.whitePawns, 'p', WHITE, moves, bestScore, bestMove);
    } else {
        evalMoves(b, b.blackKing, 'k', BLACK, moves, bestScore, bestMove);
        evalMoves(b, b.blackQueens, 'q', BLACK, moves, bestScore, bestMove);
        evalMoves(b, b.blackRooks, 'r', BLACK, moves, bestScore, bestMove);
        evalMoves(b, b.blackBishops, 'b', BLACK, moves, bestScore, bestMove);
        evalMoves(b, b.blackKnights, 'n', BLACK, moves, bestScore, bestMove);
        evalMoves(b, b.blackPawns, 'p', BLACK, moves, bestScore, bestMove);
    }
    if(bestScore == 0) return FindRandomMove(b); // just pick something random if no captures

    return bestMove;
}

Move Opponent::FindRandomMove(Board &b){
    auto moves = b.getAllMoves();
    int i = rand() % moves.size();

    return moves[i];
}
