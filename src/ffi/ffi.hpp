#pragma once

#include <cstdint>
#include <memory>
#include "rust/cxx.h"

class Board;

namespace ffi {

struct Piece;

class RustBoard {
public:
    RustBoard();
    ~RustBoard();
    rust::Vec<Piece> get_board();
    rust::Vec<Piece> piece_clicked(uint32_t spot);
private:
    Board *board;
};

std::unique_ptr<RustBoard> make_board();

} // namespace ffi
