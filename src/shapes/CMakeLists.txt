set(RAY_TRACER_SHAPES_SOURCE
  Sphere.cpp
  Cylinder.cpp
  Disk.cpp
  Cone.cpp
)

set(SHAPES_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/shapes)

set(RAY_TRACER_SHAPES_HEADERS
  ${SHAPES_INCLUDE_DIR}/Sphere.hpp
  ${SHAPES_INCLUDE_DIR}/Cylinder.hpp
  ${SHAPES_INCLUDE_DIR}/Disk.hpp
  ${SHAPES_INCLUDE_DIR}/Cone.hpp
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
