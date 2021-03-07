set(RAY_TRACER_CORE_SOURCE
  core.cpp

  math/Math.cpp
  math/Matrix4x4.cpp

  transformations/Quaternion.cpp
  transformations/TRS.cpp
  transformations/Transformation.cpp
  transformations/AnimatedTransformation.cpp

  RayDifferential.cpp
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

  ${CORE_HEADERS_DIR}/math/Fwd.hpp
  ${CORE_HEADERS_DIR}/math/Vector2.hpp
  ${CORE_HEADERS_DIR}/math/Vector2Impl.hpp
  ${CORE_HEADERS_DIR}/math/Vector3.hpp
  ${CORE_HEADERS_DIR}/math/Vector3Impl.hpp
  ${CORE_HEADERS_DIR}/math/Point2.hpp
  ${CORE_HEADERS_DIR}/math/Point2Impl.hpp
  ${CORE_HEADERS_DIR}/math/Point3.hpp
  ${CORE_HEADERS_DIR}/math/Point3Impl.hpp
  ${CORE_HEADERS_DIR}/math/Normal3.hpp
  ${CORE_HEADERS_DIR}/math/Normal3Impl.hpp
  ${CORE_HEADERS_DIR}/math/Matrix4x4.hpp
  ${CORE_HEADERS_DIR}/math/Interval.hpp

  # geometry
  ${CORE_HEADERS_DIR}/BoundingSphere.hpp
  ${CORE_HEADERS_DIR}/Bounds2.hpp
  ${CORE_HEADERS_DIR}/Bounds2Impl.hpp
  ${CORE_HEADERS_DIR}/Bounds2Iterator.hpp
  ${CORE_HEADERS_DIR}/Bounds3.hpp
  ${CORE_HEADERS_DIR}/Bounds3Impl.hpp
  ${CORE_HEADERS_DIR}/Geometry.hpp
  ${CORE_HEADERS_DIR}/Ray.hpp
  ${CORE_HEADERS_DIR}/RayDifferential.hpp

  ${CORE_HEADERS_DIR}/transformations/Quaternion.hpp
  ${CORE_HEADERS_DIR}/transformations/TRS.hpp
  ${CORE_HEADERS_DIR}/transformations/Transformation.hpp
  ${CORE_HEADERS_DIR}/transformations/TransformationImpl.hpp
  ${CORE_HEADERS_DIR}/transformations/AnimatedTransformation.hpp

  ${CORE_HEADERS_DIR}/Interaction.hpp
  ${CORE_HEADERS_DIR}/SurfaceInteraction.hpp
  ${CORE_HEADERS_DIR}/Shape.hpp
  ${CORE_HEADERS_DIR}/EFloat.hpp
  ${CORE_HEADERS_DIR}/AtomicFloat.hpp
  ${CORE_HEADERS_DIR}/Texture.hpp
  ${CORE_HEADERS_DIR}/Material.hpp
  ${CORE_HEADERS_DIR}/Medium.hpp

  # primitives
  ${CORE_HEADERS_DIR}/Primitive.hpp
  ${CORE_HEADERS_DIR}/GeometricPrimitive.hpp
  ${CORE_HEADERS_DIR}/TransformedPrimitive.hpp
  
  ${CORE_HEADERS_DIR}/Optional.hpp

  # color
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