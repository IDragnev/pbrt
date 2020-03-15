cmake_minimum_required(VERSION 3.10)

project(ray-tracer
LANGUAGES CXX
)

enable_testing()

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
  add_definitions(-D RT_FLOAT_AS_DOUBLE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Visual Studio source folders
source_group(core_source  REGULAR_EXPRESSION src/core/.*)
source_group(core_headers REGULAR_EXPRESSION include/core/.*)

add_subdirectory(src/core)
add_subdirectory(app)
