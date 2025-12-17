#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("quantum_chess/src/ffi/ffi.hpp");
        pub fn get_turn() -> u32;
    }
}
