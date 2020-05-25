cmake_minimum_required(VERSION 3.10)

project(market_data)

include_directories(
   ../vendor
   ../ 
   )

add_library( market_data BarSettings.cpp Contract.cpp )
target_link_libraries( market_data )