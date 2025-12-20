#include "Opponent.hpp"
#include <_abort.h>
#include <cassert>
#include <cfloat>
#include <cstdio>
#include <cstdlib>

Node::~Node() {
    delete board;
    data.clear();
}
Node::Node(Node &n)
    : board(new Board(*n.board))
    , eval(n.eval)
    , player(n.player)
{}

void Node::generateMoves(int depth) {
    if (depth == 0) {
        eval = board->evalBoard(player);
        return;
    }

    // TODO change
    data.clear(); // I shouldn't be clearing this every time

    auto boardMoves = board->getAllMoves();
    if (boardMoves.size() == 0) {
        eval = board->evalBoard(player);
        return;
    }
    for (auto m : boardMoves) {
        Data d;
        d.m = m;

        d.board = new Node;
        d.board->board = new Board(*board);
        d.board->board->movePiece(m);
        d.board->player = player == WHITE ? BLACK : WHITE;

        data.push_back(d);
    }

    for (int i = 0; i < data.size(); i++) {
        data[i].board->generateMoves(depth - 1);
    }

    // now evaluate all the moves we can
    float maxEval = FLT_MAX;
    for (auto &d : data) {
        // a move is good if it makes its respective colors eval high
        // you expect your opponent to make the move that makes your eval the lowest
        // when we get here all children have done their eval where Node::eval is the score that their best move has
        // so because all of our current children are the other type, we want to make the move that makes that score the smallest
        // the eval of the current board is that score
        // so because eval is relative to the nodes color, we want the node with the smallest eval
        if (d.board->eval < maxEval) {
            maxEval = d.board->eval;
        }
    }
    // for some reason I shouldn't multiply by -1
    eval = maxEval; // to make it our best move
}


Opponent::Opponent(Board &b, Color c, OppType t)
    : color(c)
    , type(t)
{
    currentState = new Node();
    currentState->board = new Board(b);
    currentState->eval = currentState->board->evalBoard(c);
    currentState->player = currentState->board->turn;
}
Opponent::~Opponent() {
    delete currentState;
} 
void Opponent::setBoard(Board &b) {
    if (currentState->board != nullptr) delete currentState->board;
    currentState->board = new Board(b);
}

void Opponent::moveTaken(Move m) {
    if (currentState->data.size() == 0) {
        currentState->board->movePiece(m);
        currentState->player = currentState->player == WHITE ? BLACK : WHITE;
        currentState->eval = currentState->board->evalBoard(currentState->player);
        return;
    }
    for (int i = 0; i < currentState->data.size(); i++) {
        if (currentState->data[i].m == m) {
            // this is the move that was taken
            auto temp = currentState;
            currentState = new Node(*temp->data[i].board);

            delete temp;
            break;
        }
        if (i == currentState->data.size() - 1) {
            printf("A move happened that we didnt have\n");
            abort();
        }
    }
}
Move Opponent::takeTurn(){
    Move m = findBestMove();
    moveTaken(m);
    return m;
}

Move Opponent::findBestMove(){
    currentState->generateMoves(3); // right now do a depth of 2

    Move bestMove;
    float maxEval = -FLT_MAX;

    for (int i = 0; i < currentState->data.size(); i++) {
        if (currentState->data[i].board->eval > maxEval) {
            maxEval = currentState->data[i].board->eval;
            bestMove = currentState->data[i].m;
        } 
    }

    return bestMove;
}

Move Opponent::findRandomMove(){
    auto moves = currentState->board->getAllMoves();
    int i = rand() % moves.size();

    return moves[i];
}
