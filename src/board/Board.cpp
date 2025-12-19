#include "Board.hpp"
#include <iostream>

using namespace std;

Color Board::generateBitBoards(string fen){
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
    if(fen == "") fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    U64 rank = 7;
    U64 col = 0;

    string positions = fen.substr(0, fen.find(' '));
    for(int i = 0; i < positions.size(); i++){
        U64 pos = ((U64)1 << ((rank * (U64)8) + col));
        switch(fen[i]){
            case '/':
                rank--;
                col = 0;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                col += fen[i] - 48; // converts to int and adds
                break;
            case 'p': // black pawn
                blackPawns |= pos;
                col++;
                break;
            case 'r':
                blackRooks |= pos;
                col++;
                break;
            case 'n':
                blackKnights |= pos;
                col++;
                break;
            case 'b':
                blackBishops |= pos;
                col++;
                break;
            case 'q':
                blackQueens |= pos;
                col++;
                break;
            case 'k':
                blackKing |= pos;
                col++;
                break;
            case 'P':
                whitePawns |= pos;
                col++;
                break;
            case 'R':
                whiteRooks |= pos;
                col++;
                break;
            case 'N':
                whiteKnights |= pos;
                col++;
                break;
            case 'B':
                whiteBishops |= pos;
                col++;
                break;
            case 'Q':
                whiteQueens |= pos;
                col++;
                break;
            case 'K':
                whiteKing |= pos;
                col++;
                break;
        }
    }
    // other info loop
    string otherInfo = fen.substr(fen.find(' ') + 1);
    switch(otherInfo[0]){
        case 'w':
            turn = WHITE;
            break;
        case 'b':
            turn = BLACK;
            break;
        default:
            cout << otherInfo[0] << " <- got this instead" << endl;
    }
    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    for(int i = 0; i < otherInfo.find(' '); i++){
        switch(otherInfo[i]){
            case 'K':
                whiteCastleKing = true;
                break;
            case 'Q':
                whiteCastleQueen = true;
                break;
            case 'k':
                blackCastleKing = true;
                break;
            case 'q':
                blackCastleQueen = true;
                break;
        }
    }
    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    // holds enpassant square, halfmove clock, fullmove number

    // otherInfo[0] is row
    if(otherInfo[0] != '-'){
        enpassantLoc = (U64)1 << (8 * (otherInfo[0] - 'a'));
        // shifts up rows
        enpassantLoc = enpassantLoc << (otherInfo[1] - '1');
        // shifts over columns
    }

    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    halfMoves = stoi(otherInfo.substr(0, otherInfo.find(' ')));

    otherInfo.erase(otherInfo.begin(), otherInfo.begin() + otherInfo.find(' ') + 1);
    fullMoves = stoi(otherInfo.substr(0, otherInfo.find(' ')));

    // sets up pieces
    whitePieces = 0;
    blackPieces = 0;
    for(int i = 0; i < 6; i++){
        whitePieces |= *boards[i].i;
        blackPieces |= *boards[i + 6].i;
    }

    return turn;
}
void Board::reset(std::string fen){
    for(int i = 0; i < 12; i++){
        *boards[i].i = 0;
    }
    colorMask = 0;
    debugMask = 0;

    enpassantLoc = 0;

    whiteCastleKing = 0;
    whiteCastleQueen = 0;
    blackCastleKing = 0;
    blackCastleQueen = 0;

    halfMoves = 0;
    fullMoves = 0;

    while(!moves.empty()){
        moves.pop();
    }
    while(!captures.empty()){
        captures.pop();
    }
    generateBitBoards(fen);

}

Board::Board(Board &ref){
    this->whitePawns = ref.whitePawns;
    this->whiteKnights = ref.whiteKnights;
    this->whiteBishops = ref.whiteBishops;
    this->whiteRooks = ref.whiteRooks;
    this->whiteQueens = ref.whiteQueens;
    this->whiteKing = ref.whiteKing;

    this->blackPawns = ref.blackPawns;
    this->blackKnights = ref.blackKnights;
    this->blackBishops = ref.blackBishops;
    this->blackRooks = ref.blackRooks;
    this->blackQueens = ref.blackQueens;
    this->blackKing = ref.blackKing;

    this->blackPieces = ref.blackPieces;
    this->whitePieces = ref.whitePieces;

    this->enpassantLoc = ref.enpassantLoc;

    this->whiteCastleKing = ref.whiteCastleKing;
    this->whiteCastleQueen = ref.whiteCastleQueen;
    this->blackCastleKing = ref.blackCastleKing;
    this->blackCastleQueen = ref.blackCastleQueen;

    this->halfMoves = ref.halfMoves;
    this->fullMoves = ref.fullMoves;
    this->moves = ref.moves;
    this->captures = ref.captures;

    this->turn = ref.turn;
}
void Board::printLoc(U64 x){
    printf("0x%016llu\n", x);
}

void Board::printBoardState(){
    for(int i = 0; i < 12; i++){
        cout << *boards[i].i << " : " << boards[i].piece << " " << boards[i].col << endl;
    }
    cout << whiteCastleQueen << whiteCastleKing << blackCastleQueen << blackCastleKing << endl;
    cout << halfMoves << " " << fullMoves << endl;
    cout << "Moves: " << moves.size() << endl;
    cout << "Cap: " << captures.size() << endl;
}
string Board::getMove(Move m){
    string move = "";
    if(m.piece != 'p'){
        move += (m.piece - 0x20); // shifts to uppercase
    }
    move += boardToLoc(m.from);
    if(m.special & 4) move += 'x';
    move += boardToLoc(m.to);

    if(m.color == WHITE) move += 'W';
    else move += 'B';

    return move;
}
string Board::boardToLoc(U64 board){
    string loc = "";
    int sq = getLSLoc(board);
    if(sq > 32 && sq % 8 == 0) loc += 'h';
    else loc += 'a' + (sq % 8);
    loc += '1' + (sq / 8);
    return loc;
}
float Board::evalBoard(Color c){

    // basic just count the number of pieces and associate a score
    float matScore = 0;

    // go through each piece
    matScore +=     countBits(whitePawns) - countBits(blackPawns);
    matScore += 3 * (countBits(whiteKnights) - countBits(blackKnights));
    matScore += 3.1 * (countBits(whiteBishops) - countBits(blackBishops));
    matScore += 5 * (countBits(whiteRooks) - countBits(blackRooks));
    matScore += 9 * (countBits(whiteQueens) - countBits(blackQueens));
    matScore += 200 * (countBits(whiteKing) - countBits(blackKing));

    if(c == BLACK) return matScore * -1;
    return matScore;
}
int Board::countBits(U64 i){
    int num = 0;
    while(i != 0){
        i ^= (i & -i);
        num++;
    }
    return num;
}
U64 Board::flipVertical(U64 x) {
    return ( (x << 56) ) |
            ((x << 40) & rank7) |
            ((x << 24) & rank6) |
            ((x << 8) & rank5) |
            ((x >> 8) & rank4) |
            ((x >> 24) & rank3) |
            ((x >> 40) & rank2) |
            ((x >> 56));
}
