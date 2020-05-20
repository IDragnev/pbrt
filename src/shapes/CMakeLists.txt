set(RAY_TRACER_SHAPES_SOURCE
  Sphere.cpp
)

set(SHAPES_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/shapes)

set(RAY_TRACER_SHAPES_HEADERS
  ${SHAPES_INCLUDE_DIR}/Sphere.hpp
)

add_library(
  shapeslib 
  STATIC
  ${RAY_TRACER_SHAPES_SOURCE}
  ${RAY_TRACER_SHAPES_HEADERS}
)
target_include_directories(shapeslib PUBLIC ${SHAPES_INCLUDE_DIR})
target_link_libraries(shapeslib PRIVATE corelib)
target_compile_features(shapeslib PUBLIC cxx_std_17)
