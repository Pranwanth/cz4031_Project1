cmake_minimum_required(VERSION 3.23)
project(LmaoDB)

SET(CMAKE_CXX_FLAGS "-O0 -g")
SET(CMAKE_C_FLAGS "-O0 -g")

set(CMAKE_CXX_STANDARD 20)

include_directories(BPTree)
add_executable(LmaoDB
        storage.h
        storage.cpp
        BPTree.h
        BPTreeLeafNode.tpp
        BPTreeRegularNode.tpp
        test.cpp
        )
