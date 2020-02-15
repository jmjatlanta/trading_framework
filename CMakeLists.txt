cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MyTradingSystem)

add_subdirectory(ib)
add_subdirectory(risk_management)
add_subdirectory(test)
add_subdirectory(store)

add_executable(MyTradingSystem main.cpp)