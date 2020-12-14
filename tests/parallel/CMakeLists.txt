add_executable(parallel_test
  parallel.cpp
)
target_link_libraries(parallel_test parallel doctest)
target_compile_options(parallel_test
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)