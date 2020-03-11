set(RAY_TRACER_CORE_SOURCE
  Ray.cpp
  RayDifferential.cpp
)

set(RAY_TRACER_CORE_HEADERS
  ${PROJECT_SOURCE_DIR}/include/core/core.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Vector2.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Vector2Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Vector3.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Vector3Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Point2.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Point2Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Point3.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Point3Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Normal3.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Normal3Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/BoundingSphere.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Bounds2.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Bounds2Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Bounds2Iterator.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Bounds3.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Bounds3Impl.hpp
  ${PROJECT_SOURCE_DIR}/include/core/Ray.hpp
  ${PROJECT_SOURCE_DIR}/include/core/RayDifferential.hpp
)

# core library
add_library(
  corelib 
  STATIC
  ${RAY_TRACER_CORE_SOURCE}
  ${RAY_TRACER_CORE_HEADERS}
)
target_include_directories(corelib PUBLIC ${PROJECT_SOURCE_DIR}/include/core/)
target_compile_features(corelib PUBLIC cxx_std_17)
