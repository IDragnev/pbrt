set(RAY_TRACER_CORE_SOURCE
  Ray.cpp
  RayDifferential.cpp
  Matrix4x4.cpp
  Transformation.cpp
  Quaternion.cpp
  TRS.cpp
  AnimatedTransformation.cpp
  Interval.cpp
)

set(CORE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/core)

set(RAY_TRACER_CORE_HEADERS
  ${CORE_INCLUDE_DIR}/core.hpp
  ${CORE_INCLUDE_DIR}/Vector2.hpp
  ${CORE_INCLUDE_DIR}/Vector2Impl.hpp
  ${CORE_INCLUDE_DIR}/Vector3.hpp
  ${CORE_INCLUDE_DIR}/Vector3Impl.hpp
  ${CORE_INCLUDE_DIR}/Point2.hpp
  ${CORE_INCLUDE_DIR}/Point2Impl.hpp
  ${CORE_INCLUDE_DIR}/Point3.hpp
  ${CORE_INCLUDE_DIR}/Point3Impl.hpp
  ${CORE_INCLUDE_DIR}/Normal3.hpp
  ${CORE_INCLUDE_DIR}/Normal3Impl.hpp
  ${CORE_INCLUDE_DIR}/BoundingSphere.hpp
  ${CORE_INCLUDE_DIR}/Bounds2.hpp
  ${CORE_INCLUDE_DIR}/Bounds2Impl.hpp
  ${CORE_INCLUDE_DIR}/Bounds2Iterator.hpp
  ${CORE_INCLUDE_DIR}/Bounds3.hpp
  ${CORE_INCLUDE_DIR}/Bounds3Impl.hpp
  ${CORE_INCLUDE_DIR}/Ray.hpp
  ${CORE_INCLUDE_DIR}/RayDifferential.hpp
  ${CORE_INCLUDE_DIR}/Matrix4x4.hpp
  ${CORE_INCLUDE_DIR}/Transformation.hpp
  ${CORE_INCLUDE_DIR}/TransformationImpl.hpp
  ${CORE_INCLUDE_DIR}/Quaternion.hpp
  ${CORE_INCLUDE_DIR}/TRS.hpp
  ${CORE_INCLUDE_DIR}/AnimatedTransformation.hpp
  ${CORE_INCLUDE_DIR}/Interval.hpp
)

add_library(
  corelib 
  STATIC
  ${RAY_TRACER_CORE_SOURCE}
  ${RAY_TRACER_CORE_HEADERS}
)
target_include_directories(corelib PUBLIC ${CORE_INCLUDE_DIR})
target_compile_features(corelib PUBLIC cxx_std_17)
