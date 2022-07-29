cmake_minimum_required(VERSION 3.10)

find_package(CURL REQUIRED ) 

project(tf_tests)

include_directories(../vendor
   ../
   ../vendor/date/include
   ../vendor/rapidjson/thirdparty/gtest
   ${RapidJSON_INCLUDE_DIRS}
   ${CURL_INCLUDE_DIRS} )

add_executable( tf_tests
    ibwrapper_test.cpp 
    strategy_test.cpp 
    backtester_test.cpp
    util_test.cpp
    test_domain.cpp
    ../vendor/date/src/tz.cpp )
target_link_libraries(tf_tests ib backtest domain market_data gtest_main ${CURL_LIBRARIES} )