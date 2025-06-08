cmake_minimum_required(VERSION 3.8)
project(psirngclient)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS_INIT} -fPIC")

add_subdirectory(src)
add_subdirectory(example)