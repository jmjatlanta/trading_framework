cmake_minimum_required(VERSION 3.10)

project(simple_start)

include_directories(../vendor
   ../ 
   ${Boost_INCLUDE_DIRS})

add_executable( simple_start SimpleStart.cpp )
target_link_libraries(simple_start backtest domain market_data date ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})