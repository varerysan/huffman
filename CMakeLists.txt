cmake_minimum_required(VERSION 3.8)

#set(CMAKE_OSX_ARCHITECTURES "x86_64;i386")
#set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_OSX_ARCHITECTURES "i386")

set(PROJECT_NAME_STR huffman)
project(${PROJECT_NAME_STR} CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(huffman src/main.cpp)
