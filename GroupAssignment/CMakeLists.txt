cmake_minimum_required(VERSION 3.29)
project(GroupAssignment)

set(CMAKE_CXX_STANDARD 20)

add_executable(GroupAssignment main.cpp
        wattz_strogatz.cpp
        wattz_strogatz.h
        Barabasi.cpp
        Barabasi.h
        Graph.cpp
        Graph.h)
