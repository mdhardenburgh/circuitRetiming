add_library(adjMatrix STATIC adjMatrix.cpp)

target_include_directories(adjMatrix PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)