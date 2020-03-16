add_executable(core_test 
core.cpp
bounds2.cpp
bounds3.cpp
)
target_link_libraries(core_test corelib doctest)