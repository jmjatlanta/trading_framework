cmake_minimum_required(VERSION 3.10)

find_package(Boost 1.65 COMPONENTS unit_test_framework REQUIRED)
find_package(CURL REQUIRED ) 

project(tests)

include_directories(../vendor
   ../
   ../vendor/date/include
   ${Boost_INCLUDE_DIRS}
   ${CURL_INCLUDE_DIRS} )

add_executable( tests test_main.cpp ibwrapper_test.cpp strategy_test.cpp backtester_test.cpp ../vendor/date/src/tz.cpp )
target_link_libraries(tests ib backtest domain market_data ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${CURL_LIBRARIES} )