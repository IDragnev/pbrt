add_executable(filters_test filters.cpp)
target_link_libraries(filters_test filters doctest)
target_compile_options(filters_test PRIVATE ${PBRT_TARGET_WARNING_FLAGS})