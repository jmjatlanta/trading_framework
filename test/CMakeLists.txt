cmake_minimum_required(VERSION 3.10)

find_package(Boost 1.70 COMPONENTS unit_test_framework REQUIRED)

project(tests)

include_directories(../vendor
   ../ 
   ${Boost_INCLUDE_DIRS})

add_executable(tests ibwrapper_test.cpp)
target_link_libraries(tests ib)