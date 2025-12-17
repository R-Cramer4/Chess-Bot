cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)

set(LIB_NAME ChessBoard)
project(${LIB_NAME})
find_package(glm REQUIRED)

add_library(${LIB_NAME})

set(SOURCES 
    Test.cpp
    Board.cpp
    Game.cpp
    Move.cpp
    Opponent.cpp
)
set(HEADERS
    Test.hpp
    Board.hpp
    Game.hpp
    Masks.hpp
    Opponent.hpp
)
target_sources(${LIB_NAME} PRIVATE ${SOURCES} ${HEADERS})


target_link_libraries(${LIB_NAME} PUBLIC glm::glm)

install(TARGETS ${LIB_NAME})
