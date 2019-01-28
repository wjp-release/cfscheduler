aux_source_directory(. TESTDIRSRCS)
add_executable(tests ${TESTDIRSRCS})
target_link_libraries(tests gtest_main cfsched)
