cmake_minimum_required(VERSION 3.10)

project(ray-tracer)

enable_testing()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_definitions(-DNDEBUG)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(RAY_TRACER_CORE_SOURCE
  src/core/Ray.cpp
  src/core/RayDifferential.cpp
)

set(RAY_TRACER_CORE_HEADERS
  src/core/core.hpp
  src/core/Vector2.hpp
  src/core/Vector2Impl.hpp
  src/core/Vector3.hpp
  src/core/Vector3Impl.hpp
  src/core/Point2.hpp
  src/core/Point2Impl.hpp
  src/core/Point3.hpp
  src/core/Point3Impl.hpp
  src/core/Normal3.hpp
  src/core/Normal3Impl.hpp
  src/core/BoundingSphere.hpp
  src/core/Bounds2.hpp
  src/core/Bounds2Impl.hpp
  src/core/BoundsIterator.hpp
  src/core/Ray.hpp
  src/core/RayDifferential.hpp
)

# Visual Studio source folders
source_group(core REGULAR_EXPRESSION src/core/.*)

# core library
add_library(
  corelib 
  STATIC
  ${RAY_TRACER_CORE_SOURCE}
  ${RAY_TRACER_CORE_HEADERS}
)
target_include_directories(corelib PRIVATE src/)
target_include_directories(corelib PRIVATE src/core/)
target_compile_features(corelib PRIVATE cxx_std_17)

set(ALL_RAY_TRACER_LIBS
  corelib
)

# main program
add_executable(raytracer_exe src/main.cpp)
set_target_properties(raytracer_exe PROPERTIES OUTPUT_NAME raytracer)
target_compile_features(raytracer_exe PRIVATE cxx_std_17)
target_link_libraries(raytracer_exe ${ALL_RAY_TRACER_LIBS})


install(TARGETS raytracer_exe DESTINATION bin)
install(TARGETS corelib DESTINATION lib)