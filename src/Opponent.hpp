#ifndef OPP_HPP
#define OPP_HPP

#include "Board.hpp"


class Opponent{

public:
    Opponent(Color c){
        this->c = c;
    }
    Color c;

    int takeTurn(Board &b);

    Move FindRandomMove(Board &b, vector<Move> &moves);

};



#endif
