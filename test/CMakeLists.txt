cmake_minimum_required(VERSION 3.10)

project(tests)

include_directories(../vendor
   ../ )

add_executable(tests ibwrapper_test.cpp)
target_link_libraries(tests ib)