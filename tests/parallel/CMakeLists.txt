add_executable(parallel_test
  parallel.cpp
)
target_link_libraries(parallel_test parallel doctest)