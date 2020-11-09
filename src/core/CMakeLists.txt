find_package(Threads)

set(RAY_TRACER_CORE_SOURCE
  core.cpp
  Ray.cpp
  RayDifferential.cpp
  Matrix4x4.cpp
  Transformation.cpp
  Quaternion.cpp
  TRS.cpp
  AnimatedTransformation.cpp
  Interval.cpp
  SurfaceInteraction.cpp
  Shape.cpp
  EFloat.cpp
  Parallel.cpp
)

set(CORE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/core)

set(RAY_TRACER_CORE_HEADERS
  ${CORE_HEADERS_DIR}/core.hpp
  ${CORE_HEADERS_DIR}/Functional.hpp
  ${CORE_HEADERS_DIR}/Vector2.hpp
  ${CORE_HEADERS_DIR}/Vector2Impl.hpp
  ${CORE_HEADERS_DIR}/Vector3.hpp
  ${CORE_HEADERS_DIR}/Vector3Impl.hpp
  ${CORE_HEADERS_DIR}/Point2.hpp
  ${CORE_HEADERS_DIR}/Point2Impl.hpp
  ${CORE_HEADERS_DIR}/Point3.hpp
  ${CORE_HEADERS_DIR}/Point3Impl.hpp
  ${CORE_HEADERS_DIR}/Normal3.hpp
  ${CORE_HEADERS_DIR}/Normal3Impl.hpp
  ${CORE_HEADERS_DIR}/BoundingSphere.hpp
  ${CORE_HEADERS_DIR}/Bounds2.hpp
  ${CORE_HEADERS_DIR}/Bounds2Impl.hpp
  ${CORE_HEADERS_DIR}/Bounds2Iterator.hpp
  ${CORE_HEADERS_DIR}/Bounds3.hpp
  ${CORE_HEADERS_DIR}/Bounds3Impl.hpp
  ${CORE_HEADERS_DIR}/Ray.hpp
  ${CORE_HEADERS_DIR}/RayDifferential.hpp
  ${CORE_HEADERS_DIR}/Matrix4x4.hpp
  ${CORE_HEADERS_DIR}/Transformation.hpp
  ${CORE_HEADERS_DIR}/TransformationImpl.hpp
  ${CORE_HEADERS_DIR}/Quaternion.hpp
  ${CORE_HEADERS_DIR}/TRS.hpp
  ${CORE_HEADERS_DIR}/AnimatedTransformation.hpp
  ${CORE_HEADERS_DIR}/Interval.hpp
  ${CORE_HEADERS_DIR}/Interaction.hpp
  ${CORE_HEADERS_DIR}/SurfaceInteraction.hpp
  ${CORE_HEADERS_DIR}/Shape.hpp
  ${CORE_HEADERS_DIR}/EFloat.hpp
  ${CORE_HEADERS_DIR}/Texture.hpp
  ${CORE_HEADERS_DIR}/Parallel.hpp
)

add_library(
  corelib 
  STATIC
  ${RAY_TRACER_CORE_SOURCE}
  ${RAY_TRACER_CORE_HEADERS}
)
target_link_libraries(
  corelib
  PRIVATE ${CMAKE_THREAD_LIBS_INIT}
)
target_include_directories(corelib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(corelib PUBLIC cxx_std_17)
