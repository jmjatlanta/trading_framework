cmake_minimum_required(VERSION 3.10)

project(MyTradingSystem)

add_subdirectory(ib)
add_subdirectory(test)

add_executable(MyTradingSystem main.cpp)