add_executable(functional_test functional.cpp)
target_link_libraries(functional_test functional doctest)
target_compile_options(functional_test
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)