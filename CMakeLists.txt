cmake_minimum_required(VERSION 3.20)

project(MiniC VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_DEBUG_LOG "Enable debug logging" OFF)

add_executable(minic
    src/main.cpp
    src/lexer/lexer.cpp
    src/lexer/lexer_tables.cpp
    src/common/debug.cpp
    src/parser/parser.cpp
    src/parser/topLevelParser.cpp
    src/parser/statementParser.cpp
    src/parser/expressionParser.cpp
)

target_include_directories(minic PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_options(minic PRIVATE
    -Wall
    -Wextra
)

if(ENABLE_DEBUG_LOG)
    target_compile_definitions(minic PRIVATE ENABLE_DEBUG_LOG)
endif()

find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_custom_target(test-lexer
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/test_lexer.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS minic
    COMMENT "Running lexer tests"
)

add_custom_target(test-parser
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/test_parser.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS minic
    COMMENT "Running parser tests"
)

add_custom_target(test
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/run_tests.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS minic
    COMMENT "Running all test suites"
)
