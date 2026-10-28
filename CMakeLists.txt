#cmake -S . -B build
#cmake --build build

cmake_minimum_required(VERSION 3.23.1)
set (CMAKE_CXX_STANDARD 17)
project(retimeCircuit VERSION 1.0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(retimer)
add_subdirectory(verbosity)
add_subdirectory(adjMatrix)
add_subdirectory(parser)

add_executable(retimeCircuit main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -c -g -std=c++17 -Wall -W -Werror -pedantic)
target_link_libraries(retimeCircuit PRIVATE 
    retimer
    parser
    verbosity
    adjMatrix
)