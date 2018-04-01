cmake_minimum_required(VERSION 3.5)
project(poker_assignment)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../OMPEval" "${CMAKE_BINARY_DIR}/OMPEval")

add_executable(poker_assignment main.cpp)
target_link_libraries(poker_assignment libOMPEval)
