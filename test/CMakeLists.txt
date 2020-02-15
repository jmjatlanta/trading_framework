cmake_minimum_required(VERSION 3.10)

find_package(Boost 1.65 COMPONENTS unit_test_framework REQUIRED)

project(tests)

include_directories(../vendor
   ../ 
   ${Boost_INCLUDE_DIRS})

add_executable( tests test_main.cpp ibwrapper_test.cpp strategy_test.cpp )
target_link_libraries(tests ib risk_management ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})