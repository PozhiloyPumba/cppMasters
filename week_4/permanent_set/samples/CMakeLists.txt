add_executable(main main.cpp)
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/${INCLUDE_DIR})
target_link_libraries(main PUBLIC persistentSpec)