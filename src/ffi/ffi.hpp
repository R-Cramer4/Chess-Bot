#pragma once

#include <cstdint>
#include <memory>
#include "rust/cxx.h"

class Game;

namespace ffi {

struct Piece;

class RustBoard {
public:
    RustBoard();
    ~RustBoard();
    rust::Vec<Piece> get_board();
    bool piece_clicked(uint32_t spot);
private:
    Game *game;
};

std::unique_ptr<RustBoard> make_board();

} // namespace ffi
