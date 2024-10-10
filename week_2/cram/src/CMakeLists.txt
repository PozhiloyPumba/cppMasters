file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
FLEX_TARGET(scanner lexer/lex.l  ${CMAKE_CURRENT_BINARY_DIR}/generated/lexer.cpp)

set(SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/generated/lexer.cpp
    lexer/lexer.cpp
)

add_library(lexer_lib ${SOURCES})
target_include_directories(lexer_lib PUBLIC ${CMAKE_SOURCE_DIR}/${INCLUDE_DIR})

add_library(interpreter_lib interpreter.cpp)
target_link_libraries(interpreter_lib lexer_lib)