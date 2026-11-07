cmake_minimum_required(VERSION 3.20)

project(LoanCalculatorLab4 VERSION 1.0 LANGUAGES CXX)

# C++ standard configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GoogleTest dependency
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

# Main loan calculator module
add_library(loan_calculator
    src/loan_calculator.cpp
)

target_include_directories(loan_calculator
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Unit test executable
enable_testing()

add_executable(loan_calculator_tests
    tests/test_loan_calculator.cpp
)

target_link_libraries(loan_calculator_tests
    PRIVATE
    loan_calculator
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(loan_calculator_tests)

# GCC coverage configuration
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(loan_calculator PRIVATE
        --coverage
        -O0
        -g
    )

    target_compile_options(loan_calculator_tests PRIVATE
        --coverage
        -O0
        -g
    )

    target_link_options(loan_calculator_tests PRIVATE
        --coverage
    )
endif()
