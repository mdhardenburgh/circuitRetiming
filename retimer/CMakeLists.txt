add_library(retimer STATIC retimer.cpp)

target_include_directories(retimer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(retimer PUBLIC verbosity adjMatrix)