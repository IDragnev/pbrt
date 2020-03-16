add_executable(core_test 
	core/core.cpp
	core/bounds2.cpp
	core/bounds3.cpp
)
target_link_libraries(core_test corelib doctest)