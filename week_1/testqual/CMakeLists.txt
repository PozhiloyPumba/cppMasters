cmake_minimum_required (VERSION 3.10)

project (TestQual VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT WIN32)
    set(WIN32 0)
endif()

if(MSVC)
    add_compile_options("/W3" "$<$<CONFIG:RELEASE>:/O2>")
else()
    add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O2>")
endif()

find_package(FLEX REQUIRED)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
FLEX_TARGET(scanner lex.l  ${CMAKE_CURRENT_BINARY_DIR}/generated/lexer.cpp)

add_executable(testqual testqual.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated/lexer.cpp)
target_include_directories(testqual PUBLIC .)