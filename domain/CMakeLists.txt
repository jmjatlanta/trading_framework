cmake_minimum_required(VERSION 3.10)

project(domain)

include_directories(
   ../vendor
   ../ 
   )

add_library( domain Trade.cpp Strategy.cpp Contract.cpp)
target_link_libraries( domain )