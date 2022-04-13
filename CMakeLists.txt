cmake_minimum_required(VERSION 3.0)

# Namely, Super Taxi Route Analysor on Windows.
project("straw")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(src)
add_subdirectory(libs)