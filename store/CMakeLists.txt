cmake_minimum_required(VERSION 3.10)

project(SecurityBackfill)

include_directories(../vendor
   ../
   )

add_executable( SecurityBackfill SecurityBackfill.cpp )
target_link_libraries(SecurityBackfill ib domain market_data)