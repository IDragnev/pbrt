set(SAMPLERS_SOURCE_FILES
  StratifiedSampler.cpp
  HaltonSampler.cpp
)

set(SAMPLERS_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/samplers)
set(SAMPLERS_HEADER_FILES
  ${SAMPLERS_HEADERS_DIR}/StratifiedSampler.hpp
  ${SAMPLERS_HEADERS_DIR}/HaltonSampler.hpp
)

add_library(
  samplers
  STATIC
  ${SAMPLERS_HEADER_FILES}
  ${SAMPLERS_SOURCE_FILES}
)
target_link_libraries(samplers
  PRIVATE corelib
)
target_include_directories(samplers PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(samplers PUBLIC cxx_std_20)
target_compile_options(samplers PRIVATE ${PBRT_TARGET_WARNING_FLAGS})