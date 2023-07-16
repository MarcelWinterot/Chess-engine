cmake_minimum_required(VERSION 3.25)
project(chess_engine)

set(CMAKE_CXX_STANDARD 20)

# Add the main executable target
add_executable(chess_engine main.cpp ./thc.cpp endgameTable.cpp)

## Add the tests executable target
#add_executable(tests tests.cpp)