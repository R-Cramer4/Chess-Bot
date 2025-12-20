#ifndef OPP_HPP
#define OPP_HPP

#include "Board.hpp"
#include <vector>

enum OppType {RAND, AI, MINMAX, ALGO};

struct Node;
struct Data {
    Move m; // a move to take
    Node *board; // the board state after that move
};

struct Node {
    Board *board{ nullptr }; // current board state at this node
    // TODO make this an array
    std::vector<Data> data;

    // evaluation of the current node relative to its color
    // it is either the evaluation of the best move it could play, or the current board state
    float eval{ 0.0 };
    Color player{ WHITE }; // the current players move

    Node() = default;
    Node(Node &n);
    ~Node();
    void generateMoves(int depth);
};

class Opponent{
public:
    explicit Opponent(Board &b, Color c, OppType t = ALGO);
    ~Opponent();

    void moveTaken(Move m);
    Move takeTurn();
    void setBoard(Board &b);

    Color color{ BLACK };
private:
    OppType type{ ALGO };
    Node *currentState;


    Move findRandomMove();

    Move findBestMove();
};


#endif
