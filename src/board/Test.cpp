#include "Test.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>


void Test(string input, string output){
    cout << "Testing" << endl;

    ifstream in(input);
    ofstream out(output);

    if(!in.is_open()){
        cout << "input didn't open" << endl;
    }
    if(!out.is_open()){
        cout << "output didn't open" << endl;
    }

    int numSucceed = 0;
    string inString, outString;

    // time taken
    chrono::time_point<chrono::steady_clock> fullTestStart = chrono::steady_clock::now();
    unsigned long long positionsCalculated = 0;
    int i = 0;
    while(getline(in, inString)){
        // time each test
        chrono::time_point<chrono::steady_clock> testStart = chrono::steady_clock::now();
        i++;
        testBoard temp = parseString(inString); // get testBoard
        testBoard res = perftTest(temp); // test the testBoard and get result
        outString = makeString(res); // turn into string
        // probably should check results
        out << outString << endl; // output

        numSucceed += printTestResult(temp, res, i);
        auto milliseconds = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - testStart).count();
        long long int positions = res.d1 + res.d2 + res.d3 + res.d4 + res.d5 + res.d6;
        long long int positionsPerSecond = (double)positions / ((double) milliseconds / 1000.0);
        positionsCalculated += positions;
        cout << "Test " << i << " took " << milliseconds << "ms, " << positionsPerSecond << " positions per second" << endl;
    }

    in.close();
    out.close();

    if(numSucceed == i){
        cout << "\033[1;32mAll Tests Passed\033[0m\n";
    }else if(numSucceed == 0){
        cout << "\033[1;31mAll Tests Failed\033[0m\n";
    }else{
        cout << "\033[1m" << numSucceed << "/" << i << " of tests passed\033[0m\n";
    }
    // time full tests
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - fullTestStart).count();
    long long int positionsPerSecond = (double)positionsCalculated / ((double) milliseconds / 1000.0);
    cout << "Tests took " << milliseconds << "ms, an average of " << positionsPerSecond << " per second" << endl;
}

testBoard perftTest(testBoard in){
    testBoard out;
    out.fen = in.fen;

    int max = 6;
    if(in.d6 == -1) max = 5;

    Board b;
    b.generateBitBoards(in.fen);

    // TODO Multithread here

    out.d1 = b.Perft(1);
    b.reset(in.fen);

    out.d2 = b.Perft(2);
    b.reset(in.fen);

    out.d3 = b.Perft(3);
    b.reset(in.fen);

    out.d4 = b.Perft(4);
    b.reset(in.fen);

    out.d5 = b.Perft(5);
    b.reset(in.fen);

    if(in.d6 != -1){
        out.d6 = b.Perft(6);
        b.reset(in.fen);
    }else{
        out.d6 = -1;
    }

    return out;
}

// with limited testing this works
testBoard parseString(string str){
    testBoard b;

    b.fen = str.substr(0, str.find(';') - 1);
    str.erase(0, str.find(';') + 1);

    b.d1 = stoi(str.substr(2, str.find(';') - 1));
    str.erase(0, str.find(';') + 1);
    b.d2 = stoi(str.substr(2, str.find(';') - 1));
    str.erase(0, str.find(';') + 1);
    b.d3 = stoi(str.substr(2, str.find(';') - 1));
    str.erase(0, str.find(';') + 1);
    b.d4 = stoi(str.substr(2, str.find(';') - 1));
    str.erase(0, str.find(';') + 1);
    b.d5 = stoi(str.substr(2, str.find(';') - 1));
    str.erase(0, str.find(';') + 1);
    if(str.find(';') != -1){
        b.d6 = stoi(str.substr(2, str.find(';') - 1));
        str.erase(0, str.find(';') + 1);
    }else{
        b.d6 = -1;
    }
    return b;
}
string makeString(testBoard b){
    string out = b.fen + 
        " ;D1 " + to_string(b.d1) + 
        " ;D2 " + to_string(b.d2) + 
        " ;D3 " + to_string(b.d3) + 
        " ;D4 " + to_string(b.d4) + 
        " ;D5 " + to_string(b.d5);
    
    if(b.d6 == -1) return out;

    out += " ;D6 " + to_string(b.d6);
    return out;
}
bool printTestResult(testBoard in, testBoard out, int num){
    cout << "\033[1mTest " << num << ":\033[0m\n"; // bolded
    bool succeed = true;
    if(in.d1 != out.d1){
        succeed = false;
        cout << "\tD1:\t" << in.d1 << " : " << out.d1 << "\n";
    }
    if(in.d2 != out.d2){
        succeed = false;
        cout << "\tD2:\t" << in.d2 << " : " << out.d2 << "\n";
    }
    if(in.d3 != out.d3){
        succeed = false;
        cout << "\tD3:\t" << in.d3 << " : " << out.d3 << "\n";
    }
    if(in.d4 != out.d4){
        succeed = false;
        cout << "\tD4:\t" << in.d4 << " : " << out.d4 << "\n";
    }
    if(in.d5 != out.d5){
        succeed = false;
        cout << "\tD5:\t" << in.d5 << " : " << out.d5 << "\n";
    }
    if(in.d6 != out.d6){
        succeed = false;
        cout << "\tD6:\t" << in.d6 << " : " << out.d6 << "\n";
    }
    if(succeed){
        cout << "\033[1;32mPassed\033[0m\t\t" << in.fen << "\n";
    }else{
        cout << "\033[1;31mFailed\033[0m\t\t" << in.fen << "\n";
    }
    return succeed;
}
void printBoard(testBoard b){
    cout << b.fen << endl;
    cout << "D1: " << b.d1 << endl;
    cout << "D2: " << b.d2 << endl;
    cout << "D3: " << b.d3 << endl;
    cout << "D4: " << b.d4 << endl;
    cout << "D5: " << b.d5 << endl;
    cout << "D6: " << b.d6 << endl;

}
