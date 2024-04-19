cmake_minimum_required(VERSION 3.0)
project(huffman_encoder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

add_executable(huffman_encoder
    huffman_encoder.cpp
    compress.cpp
    expand.cpp
)