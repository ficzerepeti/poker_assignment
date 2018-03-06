cmake_minimum_required(VERSION 3.9)
project(poker_assignment)

set(CMAKE_CXX_STANDARD 17)

set(LibDivideFiles "${CMAKE_CURRENT_LIST_DIR}/OMPEval/libdivide/libdivide.h")
set(OMPEvalFiles
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/CardRange.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/CardRange.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/CombinedRange.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/CombinedRange.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/Constants.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/EquityCalculator.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/EquityCalculator.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/Hand.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/HandEvaluator.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/HandEvaluator.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/OffsetTable.hxx"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/Random.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/omp/Util.h"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/ttest/ttest.h"
        )
add_library(libOMPEval STATIC ${OMPEvalFiles} ${LibDivideFiles})

add_executable(poker_assignment main.cpp)

add_executable(ompeval_test
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/benchmark.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/OMPEval/test.cpp")
target_link_libraries(ompeval_test libOMPEval)
