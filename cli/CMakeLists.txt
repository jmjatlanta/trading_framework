cmake_minimum_required(VERSION 3.10)

project(DownloadShortingData)

set(JSON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../quandl-cpp/modules/json/src)
include_directories(
    ${QUANDLCPP_INCLUDE_DIR} 
    ${CPR_INCLUDE_DIRS} 
    ${JSON_INCLUDE_DIRS}
    ${RapidJSON_INCLUDE_DIRS})
add_executable(DownloadShortingData main.cpp)
target_link_libraries(DownloadShortingData ${QUANDLCPP_LIB} ${CPR_LIBRARIES})