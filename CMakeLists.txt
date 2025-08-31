cmake_minimum_required(VERSION 3.24)
project(Passwords)

set(CMAKE_CXX_STANDARD 17)

add_executable(Passwords proj4.cpp passserver.cpp)
