set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

aux_source_directory(. SRC_LIST)

add_executable(pnlog_test ${SRC_LIST})

target_link_libraries(pnlog_test pnlog)

add_test(NAME pnlog_test COMMAND pnlog_test)
