cmake_minimum_required(VERSION 3.10)

project(backtest)
find_package( Threads )
include_directories(
    ../vendor
    ../vendor/rapidjson/include
   ../ )

file(GLOB util_src "../util/*.cpp")

add_library(backtest BacktestStreamingService.cpp BacktestHistoricalService.cpp BacktestStrategyRunner.cpp ${util_src})
target_link_libraries(backtest ${CMAKE_THREAD_LIBS_INIT})