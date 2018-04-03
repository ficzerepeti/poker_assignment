cmake_minimum_required(VERSION 3.5)
project(poker_assignment)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../OMPEval" "${CMAKE_CURRENT_BINARY_DIR}/OMPEval")

set(CMAKE_CXX_STANDARD 17)

set(POKER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/i_my_poker_lib.h
    ${CMAKE_CURRENT_LIST_DIR}/i_user_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/my_poker_lib.h
    ${CMAKE_CURRENT_LIST_DIR}/texas_holdem_game.h
    ${CMAKE_CURRENT_LIST_DIR}/user_interaction.h)
set(POKER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/my_poker_lib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texas_holdem_game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/user_interaction.cpp)

add_library(my_poker_lib STATIC ${POKER_HEADERS} ${POKER_SOURCES})
target_link_libraries(my_poker_lib libOMPEval)
target_include_directories(my_poker_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_executable(poker_assignment main.cpp my_poker_lib.cpp)
target_link_libraries(poker_assignment my_poker_lib)
