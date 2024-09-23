add_library(test_lib test.cpp)
message(${INCLUDE_DIR})
target_include_directories(test_lib PUBLIC ${CMAKE_SOURCE_DIR}/${INCLUDE_DIR})