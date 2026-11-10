cmake_minimum_required(VERSION 3.20)

project(LoanCalculatorLab5 VERSION 1.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Code quality checks
find_program(CLANG_FORMAT_EXE NAMES clang-format)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

# Formatting check
if(CLANG_FORMAT_EXE)
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror
            ${CMAKE_CURRENT_SOURCE_DIR}/src/loan_calculator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/loan_calculator.h
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_loan_calculator.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Checking code formatting with ClangFormat"
    )
endif()

# Static analysis
if(CLANG_TIDY_EXE)
    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/src/loan_calculator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_loan_calculator.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running static analysis with Clang-Tidy"
    )
endif()
