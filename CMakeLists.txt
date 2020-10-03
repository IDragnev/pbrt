cmake_minimum_required(VERSION 3.10)

project(ray-tracer
  LANGUAGES CXX
)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_definitions(-DNDEBUG)
endif()

option(RT_FLOAT_AS_DOUBLE "Use 64-bit floats" OFF)

if(RT_FLOAT_AS_DOUBLE)
  add_definitions(-DRT_FLOAT_AS_DOUBLE)
endif()

if (MSVC)
  add_compile_options(/WX)
else()
  add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src/core)
add_subdirectory(src/shapes)
add_subdirectory(app)
add_subdirectory(external/doctest)
add_subdirectory(tests/core)