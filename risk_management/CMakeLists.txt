cmake_minimum_required(VERSION 3.10)

project(risk_management)

include_directories(
   ../vendor
   ../ 
   )

add_library( risk_management MarketMonitor.cpp Trade.cpp Strategy.cpp)
target_link_libraries( risk_management )