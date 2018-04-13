cmake_minimum_required(VERSION 3.1)
project(poker_assignment)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/externals/OMPEval")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/externals/googletest")

set(CMAKE_CXX_STANDARD 17)

set(POKER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/holdem_game_orchestrator.h
    ${CMAKE_CURRENT_LIST_DIR}/i_my_poker_lib.h
    ${CMAKE_CURRENT_LIST_DIR}/i_user_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/my_poker_lib.h
    ${CMAKE_CURRENT_LIST_DIR}/streamed_user_interaction.h
    ${CMAKE_CURRENT_LIST_DIR}/table/game_stages.h
    ${CMAKE_CURRENT_LIST_DIR}/table/holdem_table_state_manager.h
    ${CMAKE_CURRENT_LIST_DIR}/table/i_table_state_manager.h
    ${CMAKE_CURRENT_LIST_DIR}/table/player_actions.h
    ${CMAKE_CURRENT_LIST_DIR}/table/player_state.h
    ${CMAKE_CURRENT_LIST_DIR}/table/table_state.h)
set(POKER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/holdem_game_orchestrator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/my_poker_lib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/streamed_user_interaction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/table/game_stages.cpp
    ${CMAKE_CURRENT_LIST_DIR}/table/holdem_table_state_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/table/player_actions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/table/player_state.cpp
    ${CMAKE_CURRENT_LIST_DIR}/table/table_state.cpp)

add_library(my_poker_lib STATIC ${POKER_HEADERS} ${POKER_SOURCES})
target_link_libraries(my_poker_lib libOMPEval)
target_include_directories(my_poker_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if (NOT MSVC)
    target_compile_options(my_poker_lib
        PUBLIC "-Wall"
        PUBLIC "-Wextra"
        PUBLIC "-Werror"
        PUBLIC "-Wno-unused-parameter")
endif()

add_executable(texas_holdem_game ${CMAKE_CURRENT_LIST_DIR}/main_holdem_game.cpp)
target_link_libraries(texas_holdem_game my_poker_lib)

add_executable(tests ${CMAKE_CURRENT_LIST_DIR}/unit_tests/test_table.cpp)
target_link_libraries(tests gtest_main gmock_main my_poker_lib)
