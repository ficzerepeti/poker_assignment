cmake_minimum_required(VERSION 3.5)
project(poker_assignment)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../OMPEval" "${CMAKE_CURRENT_BINARY_DIR}/OMPEval")

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Werror")

set(POKER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/deck_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/i_deck_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/i_my_poker_lib.h
    ${CMAKE_CURRENT_LIST_DIR}/i_user_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/my_poker_lib.h
    ${CMAKE_CURRENT_LIST_DIR}/player_actions.h
    ${CMAKE_CURRENT_LIST_DIR}/streamed_user_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/texas_holdem_game.h )
set(POKER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/deck_interaction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/my_poker_lib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/player_actions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/streamed_user_interaction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texas_holdem_game.cpp)

add_library(my_poker_lib STATIC ${POKER_HEADERS} ${POKER_SOURCES})
target_link_libraries(my_poker_lib libOMPEval)
target_include_directories(my_poker_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_executable(poker_assignment main.cpp my_poker_lib.cpp)
target_link_libraries(poker_assignment my_poker_lib)
