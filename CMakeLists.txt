cmake_minimum_required(VERSION 3.5)
project(poker_assignment)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../OMPEval" "${CMAKE_CURRENT_BINARY_DIR}/OMPEval")

set(CMAKE_CXX_STANDARD 17)

set(POKER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/equity_calculator.h
    ${CMAKE_CURRENT_LIST_DIR}/i_equity_calculator.h)
set(POKER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/equity_calculator.cpp)

add_library(my_poker_lib STATIC ${POKER_HEADERS} ${POKER_SOURCES})
target_link_libraries(my_poker_lib libOMPEval)
target_include_directories(my_poker_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_executable(poker_assignment main.cpp equity_calculator.cpp )
target_link_libraries(poker_assignment my_poker_lib)
