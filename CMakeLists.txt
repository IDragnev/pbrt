cmake_minimum_required(VERSION 3.14)

project(ray-tracer
  LANGUAGES CXX
)

include(FetchContent)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_BUILD_TYPE MATCHES RELEASE)
  add_compile_definitions(NDEBUG)
endif()

option(PBRT_FLOAT_AS_DOUBLE "Use 64-bit floats" OFF)
option(PBRT_TREAT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(PBRT_SAMPLED_SPECTRUM "Use SampledSpectrum rather than RGBSpectrum" OFF)

if(PBRT_FLOAT_AS_DOUBLE)
  add_compile_definitions(PBRT_FLOAT_AS_DOUBLE)
endif()

if(MSVC)
  set(PBRT_TARGET_WARNING_FLAGS
    "/W4"
    $<$<BOOL:${PBRT_TREAT_WARNINGS_AS_ERRORS}>:/WX>
    "/w15038" "/w14800" "/w14242" "/w14254" "/w14263" "/w14264" "/w14265" "/w14266"
    "/w14287" "/w14296" "/w14388" "/w14471" "/w14619" "/w14800" "/w14946"
  )
else()
  set(PBRT_TARGET_WARNING_FLAGS "-Wall" "-Wextra" $<$<BOOL:${PBRT_TREAT_WARNINGS_AS_ERRORS}>:-Werror> "-pedantic")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# DEPENDENCIES 
FetchContent_Declare(tl_optional
  GIT_REPOSITORY https://github.com/TartanLlama/optional.git
)
FetchContent_GetProperties(tl_optional)
if(NOT tl_optional_POPULATED)
  FetchContent_Populate(tl_optional)
  option(OPTIONAL_BUILD_PACKAGE "" OFF)
  set(BUILD_TESTING false)
  add_subdirectory(${tl_optional_SOURCE_DIR} ${tl_optional_BINARY_DIR})
endif()

FetchContent_Declare(doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        2.4.3
)
FetchContent_MakeAvailable(doctest)

# PBRT targets
add_subdirectory(src/functional)
add_subdirectory(src/parallel)
add_subdirectory(src/memory)
add_subdirectory(src/core)
add_subdirectory(src/shapes)
add_subdirectory(src/accelerators)
add_subdirectory(src/cameras)
add_subdirectory(src/samplers)
add_subdirectory(src/filters)
add_subdirectory(app)
add_subdirectory(tests/core)
add_subdirectory(tests/memory)
add_subdirectory(tests/functional)
add_subdirectory(tests/parallel)
add_subdirectory(tests/filters)