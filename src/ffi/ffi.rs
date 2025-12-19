#[cxx::bridge(namespace = "ffi")]
pub mod ffi {
    enum PieceType {
        None,
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King,
    }
    enum Color {
        Black,
        White,
    }
    struct Piece {
        piece: PieceType,
        color: Color,
        masked: bool,
    }

    unsafe extern "C++" {
        include!("Chess-Bot/src/ffi/ffi.hpp");

        type RustBoard;
        pub fn get_board(self: Pin<&mut RustBoard>) -> Vec<Piece>;
        pub fn piece_clicked(self: Pin<&mut RustBoard>, spot: u32) -> bool;

        pub fn make_board() -> UniquePtr<RustBoard>;
    }
}
