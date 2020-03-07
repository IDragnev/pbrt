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
)

set(RAY_TRACER_CORE_HEADERS
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
target_include_directories(corelib src)
target_include_directories(corelib src/core)
target_compile_features(corelib PRIVATE cxx_std_17)

# main program
set(ALL_RAY_TRACER_LIBS
  corelib
)

add_executable(raytracer_exe src/main.cpp)
set_target_properties(raytracer_exe PROPERTIES OUTPUT_NAME raytracer)
target_compile_features(raytracer_exe PRIVATE cxx_std_17)
target_link_libraries(raytracer_exe ${ALL_RAY_TRACER_LIBS})


install(TARGETS raytracer_exe DESTINATION bin)
install(TARGETS corelib DESTINATION lib)