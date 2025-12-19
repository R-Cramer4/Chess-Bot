#include "ffi.hpp"
#include "chess-bot/src/ffi/ffi.rs.h"
#include "Chess-Bot/src/board/Game.hpp"
#include <memory>


namespace ffi {

std::unique_ptr<RustBoard> make_board() {
    return std::make_unique<RustBoard>();
}
RustBoard::RustBoard() 
    : game(new Game)
{}
RustBoard::~RustBoard() {
    delete game;
}
rust::Vec<Piece> RustBoard::get_board() {
    rust::Vec<Piece> pieces;
    U64 mask = 1;
    for (int i = 0; i < 64; i++) {
        Piece piece = Piece {
            .piece = PieceType::None,
            .color = Color::White,
            .masked = false,
        };
        if (game->bitboard.whitePawns & mask) {
            piece.piece = PieceType::Pawn;
            piece.color = Color::White;
        } else if (game->bitboard.whiteKnights & mask) {
            piece.piece = PieceType::Knight;
            piece.color = Color::White;
        } else if (game->bitboard.whiteBishops & mask) {
            piece.piece = PieceType::Bishop;
            piece.color = Color::White;
        } else if (game->bitboard.whiteRooks & mask) {
            piece.piece = PieceType::Rook;
            piece.color = Color::White;
        } else if (game->bitboard.whiteQueens & mask) {
            piece.piece = PieceType::Queen;
            piece.color = Color::White;
        } else if (game->bitboard.whiteKing & mask) {
            piece.piece = PieceType::King;
            piece.color = Color::White;
        } else if (game->bitboard.blackPawns & mask) {
            piece.piece = PieceType::Pawn;
            piece.color = Color::Black;
        } else if (game->bitboard.blackKnights & mask) {
            piece.piece = PieceType::Knight;
            piece.color = Color::Black;
        } else if (game->bitboard.blackBishops & mask) {
            piece.piece = PieceType::Bishop;
            piece.color = Color::Black;
        } else if (game->bitboard.blackRooks & mask) {
            piece.piece = PieceType::Rook;
            piece.color = Color::Black;
        } else if (game->bitboard.blackQueens & mask) {
            piece.piece = PieceType::Queen;
            piece.color = Color::Black;
        } else if (game->bitboard.blackKing & mask) {
            piece.piece = PieceType::King;
            piece.color = Color::Black;
        }

        if (game->bitboard.colorMask & mask) {
            piece.masked = true;
        }

        pieces.push_back(piece);
        mask <<= 1;
    }
    return pieces;
}
bool RustBoard::piece_clicked(uint32_t spot) {
    return game->handleClick(spot);
}


} // namespace ffi

