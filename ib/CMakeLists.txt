cmake_minimum_required(VERSION 3.10)

project(ib)
find_package( Threads )
include_directories(../vendor
   ../ )

file(GLOB ib_src "../vendor/ib_client/*.cpp")
file(GLOB util_src "../util/*.cpp")

add_library(ib IBWrapper.cpp IBHistoricalService.cpp IBStreamingService.cpp IBAccountService.cpp ${ib_src} ${util_src})
target_link_libraries(ib ${CMAKE_THREAD_LIBS_INIT})