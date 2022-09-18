cmake_minimum_required(VERSION 3.23)
project(LmaoDB)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
add_executable(LmaoDB
        BPTree.h
        BPTreeLeafNode.tpp
        BPTreeRegularNode.tpp
        test.cpp
        )
