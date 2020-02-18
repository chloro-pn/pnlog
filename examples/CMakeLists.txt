set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(example0 example0.cpp)
add_executable(example1 example1.cpp)
add_executable(example2 example2.cpp)
add_executable(example3 example3.cpp)
add_executable(example4 example4.cpp)
add_executable(example5 example5.cpp)
add_executable(example6 example6.cpp)
add_executable(example7 example7.cpp)
add_executable(piece_example piece_example.cpp)

target_link_libraries(example0 pnlog)
target_link_libraries(example1 pnlog)
target_link_libraries(example2 pnlog)
target_link_libraries(example3 pnlog)
target_link_libraries(example4 pnlog)
target_link_libraries(example5 pnlog)
target_link_libraries(example6 pnlog)
target_link_libraries(example7 pnlog)
target_link_libraries(piece_example pnlog)




