set(ACCELERATORS_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/accelerators)

set(ACCELERATORS_HEADER_FILES
  ${ACCELERATORS_HEADERS_DIR}/BVH.hpp
  ${ACCELERATORS_HEADERS_DIR}/BVHBuilders.hpp
)

set(ACCELERATORS_SOURCE_FILES
  BVH.cpp
  BVHRecursiveBuilder.cpp
)

add_library(
  acceleratorslib 
  STATIC
  ${ACCELERATORS_SOURCE_FILES}
  ${ACCELERATORS_HEADER_FILES}
)
target_link_libraries(acceleratorslib 
  PRIVATE memory
  PRIVATE corelib
  PRIVATE functional
)
target_include_directories(acceleratorslib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(acceleratorslib PUBLIC cxx_std_17)
target_compile_options(acceleratorslib
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)