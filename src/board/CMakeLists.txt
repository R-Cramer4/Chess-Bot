cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME ChessBot)
project(${PROJECT_NAME})

set(SOURCES 
    src/Test.cpp
    src/Board.cpp
    src/Game.cpp
    src/Move.cpp
    src/Opponent.cpp
)
set(HEADERS
    src/Test.hpp
    src/Board.hpp
    src/Game.cpp
    src/Masks.hpp
    src/Opponent.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(glm REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

