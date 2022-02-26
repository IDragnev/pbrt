set(REFLECTION_MODELS_SOURCE_FILES
  SpecularReflection.cpp
)

set(REFLECTION_MODELS_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/reflection_models)
set(REFLECTION_MODELS_HEADER_FILES
  ${REFLECTION_MODELS_HEADERS_DIR}/SpecularReflection.hpp
)

add_library(
  reflection_models
  STATIC
  ${REFLECTION_MODELS_HEADER_FILES}
  ${REFLECTION_MODELS_SOURCE_FILES}
)
target_link_libraries(reflection_models
  PRIVATE corelib
)
target_include_directories(reflection_models PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(reflection_models PUBLIC cxx_std_20)
target_compile_options(reflection_models PRIVATE ${PBRT_TARGET_WARNING_FLAGS})