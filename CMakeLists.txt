cmake_minimum_required(VERSION 3.1)
project(poker_assignment)

add_subdirectory("externals/OMPEval")
add_subdirectory("externals/googletest")

set(CMAKE_CXX_STANDARD 17)

set(POKER_HEADERS
    holdem_game_orchestrator.h
    i_my_poker_lib.h
    i_user_interaction.h
    my_poker_lib.h
    streamed_user_interaction.h
    table/game_stages.h
    table/holdem_table_state_manager.h
    table/i_table_state_manager.h
    table/initial_player_state.h
    table/player_actions.h
    table/player_state.h
    table/table_state.h)
set(POKER_SOURCES
    holdem_game_orchestrator.cpp
    my_poker_lib.cpp
    streamed_user_interaction.cpp
    table/game_stages.cpp
    table/holdem_table_state_manager.cpp
    table/player_actions.cpp
    table/player_state.cpp
    table/table_state.cpp)

add_library(my_poker_lib STATIC ${POKER_HEADERS} ${POKER_SOURCES})
target_link_libraries(my_poker_lib libOMPEval)
target_include_directories(my_poker_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if (MSVC)
    target_compile_definitions(my_poker_lib PUBLIC "-DNOMINMAX")
else()
    target_compile_options(my_poker_lib
        PUBLIC "-Wall"
        PUBLIC "-Wextra"
        PUBLIC "-Werror"
        PUBLIC "-Wno-unused-parameter")
endif()

add_executable(texas_holdem_game main_holdem_game.cpp)
target_link_libraries(texas_holdem_game my_poker_lib)

add_executable(tests unit_tests/test_table.cpp)
target_link_libraries(tests gtest gmock_main my_poker_lib)
