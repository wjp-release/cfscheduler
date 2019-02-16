include_directories(/usr/local/tbb/include/)
link_directories(/usr/local/tbb/lib) 

add_executable(benchmark1 benchmark1.cc)
add_executable(benchmark2 benchmark2.cc)
add_executable(benchmark3 benchmark3.cc)
add_executable(benchmark4 benchmark4.cc)
add_executable(benchmark5 benchmark5.cc)

find_package(TBB REQUIRED tbb)

target_link_libraries(benchmark1 cfsched TBB::tbb)
target_link_libraries(benchmark2 cfsched TBB::tbb)
target_link_libraries(benchmark3 cfsched TBB::tbb)
target_link_libraries(benchmark4 cfsched TBB::tbb)
target_link_libraries(benchmark5 cfsched TBB::tbb)
