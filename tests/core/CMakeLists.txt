add_executable(core_test
  main.cpp
  core.cpp
  bounds2.cpp
  bounds3.cpp
  matrix4x4.cpp
  transformation.cpp
  interval.cpp
  functional.cpp
)
target_link_libraries(core_test corelib doctest)