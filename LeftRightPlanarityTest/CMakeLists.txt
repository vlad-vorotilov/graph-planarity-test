cmake_minimum_required(VERSION 3.5)
project(LeftRightPlanarityTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Graph.h LeftRightPlanarityTest.cpp LeftRightPlanarityTest.h)
add_executable(LRTest ${SOURCE_FILES})