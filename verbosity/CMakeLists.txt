add_library(verbosity STATIC verbosity.cpp)

target_include_directories(verbosity PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)