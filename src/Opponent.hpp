#ifndef OPP_HPP
#define OPP_HPP

#include "Board.hpp"

enum oppType {RAND, AI, MINMAX};

class Opponent{

public:
    Opponent(Color c){
        this->c = c;
    }
    Opponent(Color c, oppType t){
        this->c = c;
        this->t = t;
    }
    Color c;

    int takeTurn(Board &b);


private:
    oppType t = RAND;


    Move FindRandomMove(Board &b, vector<Move> &moves);
};



#endif
