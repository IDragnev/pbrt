set(RAY_TRACER_SHAPES_SOURCE
  Sphere.cpp
  Cylinder.cpp
  Disk.cpp
  Cone.cpp
  Paraboloid.cpp
  Triangle.cpp
)

set(SHAPES_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/shapes)

set(RAY_TRACER_SHAPES_HEADERS
  ${SHAPES_HEADERS_DIR}/Sphere.hpp
  ${SHAPES_HEADERS_DIR}/Cylinder.hpp
  ${SHAPES_HEADERS_DIR}/Disk.hpp
  ${SHAPES_HEADERS_DIR}/Cone.hpp
  ${SHAPES_HEADERS_DIR}/Paraboloid.hpp
  ${SHAPES_HEADERS_DIR}/Triangle.hpp
)

add_library(
  shapeslib 
  STATIC
  ${RAY_TRACER_SHAPES_SOURCE}
  ${RAY_TRACER_SHAPES_HEADERS}
)
target_include_directories(shapeslib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(shapeslib 
  PRIVATE corelib
  PRIVATE functional
)
target_compile_features(shapeslib PUBLIC cxx_std_20)
target_compile_options(shapeslib
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)