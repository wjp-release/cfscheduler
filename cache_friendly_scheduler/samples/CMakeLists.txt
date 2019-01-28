aux_source_directory(. DIRSRCS)
add_executable(sample ${DIRSRCS})
target_link_libraries(sample cfsched)
