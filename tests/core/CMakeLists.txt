add_executable(core_test
  main.cpp
  core.cpp
  math/matrix4x4.cpp
  math/interval.cpp
  bounds2.cpp
  bounds3.cpp
  transformation.cpp
)
target_link_libraries(core_test corelib doctest)
target_compile_options(core_test
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)