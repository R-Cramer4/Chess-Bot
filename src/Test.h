#ifndef TEST_H
#define TEST_H

#include "Board.h"

using namespace std;

struct testBoard{
    string fen;
    long long int d1;
    long long int d2;
    long long int d3;
    long long int d4;
    long long int d5;
    long long int d6; // this doesn't always happen
};

void Test(string input, string output);
testBoard parseString(string str);
string makeString(testBoard b);

testBoard perftTest(testBoard in);

void printTestResult(testBoard in, testBoard out, int num);
void printBoard(testBoard b);

#endif
