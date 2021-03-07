set(RAY_TRACER_CORE_SOURCE
  core.cpp
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
  Primitive.cpp
  GeometricPrimitive.cpp
  TransformedPrimitive.cpp
  Spectrum.cpp
)

set(CORE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/core)

set(RAY_TRACER_CORE_HEADERS
  ${CORE_HEADERS_DIR}/core.hpp
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
  ${CORE_HEADERS_DIR}/Geometry.hpp
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
  ${CORE_HEADERS_DIR}/AtomicFloat.hpp
  ${CORE_HEADERS_DIR}/Texture.hpp
  ${CORE_HEADERS_DIR}/Material.hpp
  ${CORE_HEADERS_DIR}/Medium.hpp
  ${CORE_HEADERS_DIR}/Primitive.hpp
  ${CORE_HEADERS_DIR}/GeometricPrimitive.hpp
  ${CORE_HEADERS_DIR}/TransformedPrimitive.hpp
  ${CORE_HEADERS_DIR}/Optional.hpp
  ${CORE_HEADERS_DIR}/CoefficientSpectrum.hpp
  ${CORE_HEADERS_DIR}/Spectrum.hpp
)

add_library(
  corelib 
  STATIC
  ${RAY_TRACER_CORE_SOURCE}
  ${RAY_TRACER_CORE_HEADERS}
)
target_link_libraries(corelib
  PUBLIC optional
)
target_include_directories(corelib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(corelib PUBLIC cxx_std_20)
target_compile_options(corelib
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)