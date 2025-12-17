use cmake;

fn main() {
    let cmake = cmake::Config::new("src/board")
        .define("CMAKE_EXPORT_COMPILE_COMMANDS", "1")
        .always_configure(true)
        .build();


    cxx_build::bridge("src/ffi/ffi.rs")
        .file("src/ffi/ffi.cpp")
        .std("c++20")
        .compile("chessBot");

    println!("cargo:rerun-if-changed=src/ffi");
    println!("cargo:rerun-if-changed=src/board");


    println!("cargo:rustc-link-search=native={}/lib", cmake.display());
    println!("cargo:rustc-link-lib=static=ChessBoard");
}
