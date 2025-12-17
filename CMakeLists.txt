cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME ChessBot)
project(${PROJECT_NAME})

add_definitions(-DCHESS_ASSET_PATH="${CMAKE_CURRENT_BINARY_DIR}/assets")

set(SOURCES 
    src/main.cpp
    src/Test.cpp
    src/Board.cpp
    src/Game.cpp
    src/glad.c
    src/Move.cpp
    src/Opponent.cpp
    
    src/graphics/Piece.cpp
    src/graphics/Resources.cpp
    src/graphics/Shader.cpp
    src/graphics/SpriteRenderer.cpp
    src/graphics/Texture.cpp
)
set(HEADERS
    src/Test.hpp
    src/Board.hpp
    src/Game.cpp
    src/Masks.hpp
    src/Opponent.hpp
    
    src/graphics/Piece.hpp
    src/graphics/Resources.hpp
    src/graphics/Shader.hpp
    src/graphics/SpriteRenderer.hpp
    src/graphics/Texture.hpp
)

add_subdirectory(assets)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(glfw3 3.4 REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

