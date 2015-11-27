cmake_minimum_required(VERSION 3.3)
project(Exercise5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    SampleCode/client.cpp
    SampleCode/Makefile
    SampleCode/server.cpp)

add_executable(Exercise5 ${SOURCE_FILES})