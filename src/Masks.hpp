#ifndef MASK_H
#define MASK_H

#include <cstdint>

typedef uint64_t U64;
enum Color {BLACK, WHITE, NONE};
using namespace std;

const U64 aFile =         0x0101010101010101;
const U64 abFile =        0x0303030303030303;
const U64 hFile =         0x8080808080808080;
const U64 ghFile =        0xC0C0C0C0C0C0C0C0;
const U64 rank1 =         0x00000000000000FF;
const U64 rank2 =         0x000000000000FF00;
const U64 rank3 =         0x0000000000FF0000;
const U64 rank6 =         0x0000FF0000000000;
const U64 rank7 =         0x00FF000000000000;
const U64 rank8 =         0xFF00000000000000;
const U64 a1h8Diag =      0x8040201008040201;
const U64 h1a8AntiDiag =  0x0102040810204080;
const U64 lightSquares =  0x55AA55AA55AA55AA;
const U64 darkSquares =   0xAA55AA55AA55AA55;

const U64 WQPieces =      0x000000000000000E;
const U64 WKPieces =      0x0000000000000060;
const U64 BQPieces =      0x0E00000000000000;
const U64 BKPieces =      0x6000000000000000;

const U64 FullBoard =     0xFFFFFFFFFFFFFFFF;
const U64 whiteSide =     0x00000000FFFFFFFF;
const U64 blackSide =     0xFFFFFFFF00000000;

// for LSLoc function
// (Bitscan forward)
const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

// Credit for the masks, among other helpful move gen functions
// https://libres.uncg.edu/ir/asu/f/Columbia_Sophie_Spring%202023_thesis.pdf
const U64 knightMask = 0xa1100110a;
const U64 kingMask = 460039;


const U64 rankMasks[] = {0xff, 0xff00, 
                        0xff0000, 0xff000000, 
                        0xff00000000, 0xff0000000000,
                        0xff000000000000, 0xff00000000000000};

const U64 fileMasks[] = {0x8080808080808080, 0x4040404040404040,
                          0x2020202020202020, 0x1010101010101010,
                          0x0808080808080808, 0x0404040404040404,
                          0x0202020202020202, 0x0101010101010101};

const U64 diagonalMasks[] = {0x1, 0x102, 
                            0x10204, 0x1020408, 0x102040810,
                            0x10204081020, 0x1020408102040,
                            0x102040810204080, 0x204081020408000,
                            0x408102040800000, 0x810204080000000,
                            0x1020408000000000, 0x2040800000000000,
                            0x4080000000000000, 0x8000000000000000};

const U64 antiDiagonalMasks[] = {0x80, 0x8040, 0x804020, 
                                0x80402010, 0x8040201008,
                                0x804020100804, 0x80402010080402,
                                0x8040201008040201, 0x4020100804020100,
                                0x2010080402010000, 0x1008040201000000,
                                0x804020100000000, 0x402010000000000,
                                0x201000000000000, 0x100000000000000};

#endif
