set(RAY_TRACER_CORE_SOURCE
  core.cpp  
  EFloat.cpp
  SurfaceInteraction.cpp
  Shape.cpp
  Camera.cpp
  Film.cpp
  RNG.cpp

  math/Math.cpp
  math/Matrix4x4.cpp

  geometry/Ray.cpp

  transformations/Quaternion.cpp
  transformations/TRS.cpp
  transformations/Transformation.cpp
  transformations/AnimatedTransformation.cpp

  primitive/Primitive.cpp
  primitive/GeometricPrimitive.cpp
  primitive/TransformedPrimitive.cpp

  color/Spectrum.cpp

  sampling/Sampling.cpp
  sampling/Sampler.cpp
  sampling/PixelSampler.cpp
  sampling/GlobalSampler.cpp
  sampling/LowDiscrepancy.cpp
)

set(CORE_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/core)

set(RAY_TRACER_CORE_HEADERS
  ${CORE_HEADERS_DIR}/core.hpp
  ${CORE_HEADERS_DIR}/Optional.hpp
  ${CORE_HEADERS_DIR}/RNG.hpp

  ${CORE_HEADERS_DIR}/sampling/Sampling.hpp
  ${CORE_HEADERS_DIR}/sampling/Sampler.hpp
  ${CORE_HEADERS_DIR}/sampling/PixelSampler.hpp
  ${CORE_HEADERS_DIR}/sampling/GlobalSampler.hpp
  ${CORE_HEADERS_DIR}/sampling/LowDiscrepancy.hpp
  ${CORE_HEADERS_DIR}/sampling/Filter.hpp

  ${CORE_HEADERS_DIR}/math/Fwd.hpp
  ${CORE_HEADERS_DIR}/math/Math.hpp
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

  ${CORE_HEADERS_DIR}/geometry/BoundingSphere.hpp
  ${CORE_HEADERS_DIR}/geometry/Bounds2.hpp
  ${CORE_HEADERS_DIR}/geometry/Bounds2Impl.hpp
  ${CORE_HEADERS_DIR}/geometry/Bounds2Iterator.hpp
  ${CORE_HEADERS_DIR}/geometry/Bounds3.hpp
  ${CORE_HEADERS_DIR}/geometry/Bounds3Impl.hpp
  ${CORE_HEADERS_DIR}/geometry/Ray.hpp
  ${CORE_HEADERS_DIR}/geometry/Utility.hpp

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
  ${CORE_HEADERS_DIR}/Camera.hpp
  ${CORE_HEADERS_DIR}/Film.hpp

  ${CORE_HEADERS_DIR}/primitive/Primitive.hpp
  ${CORE_HEADERS_DIR}/primitive/GeometricPrimitive.hpp
  ${CORE_HEADERS_DIR}/primitive/TransformedPrimitive.hpp

  ${CORE_HEADERS_DIR}/color/CoefficientSpectrum.hpp
  ${CORE_HEADERS_DIR}/color/Spectrum.hpp
)

add_library(
  corelib 
  STATIC
  ${RAY_TRACER_CORE_SOURCE}
  ${RAY_TRACER_CORE_HEADERS}
)
target_link_libraries(corelib
  PUBLIC optional
  PRIVATE memory
)
target_include_directories(corelib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(corelib PUBLIC cxx_std_20)
target_compile_options(corelib
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)
if(PBRT_SAMPLED_SPECTRUM)
  target_compile_definitions(corelib PRIVATE PBRT_SAMPLED_SPECTRUM)
endif()