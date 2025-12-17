#include "ffi.hpp"
#include "Chess-Bot/src/board/Board.hpp"
#include <memory>


namespace ffi {

std::unique_ptr<RustBoard> make_board() {
    return std::make_unique<RustBoard>();
}
RustBoard::RustBoard() 
    : board(new Board)
{}
RustBoard::~RustBoard() {
    delete board;
}
rust::Vec<Piece> RustBoard::get_board() {

}
rust::Vec<Piece> RustBoard::piece_clicked(uint32_t spot) {

}


} // namespace ffi

