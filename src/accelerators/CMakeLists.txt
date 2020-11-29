set(ACCELERATORS_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/accelerators)

set(ACCELERATORS_HEADER_FILES
  ${ACCELERATORS_HEADERS_DIR}/BVH.hpp
)

set(ACCELERATORS_SOURCE_FILES
  BVH.cpp
)

add_library(
  acceleratorslib 
  STATIC
  ${ACCELERATORS_SOURCE_FILES}
  ${ACCELERATORS_HEADER_FILES}
)
target_link_libraries(acceleratorslib 
  PRIVATE corelib
)
target_include_directories(acceleratorslib PUBLIC ${PROJECT_SOURCE_DIR}/include/pbrt)
target_compile_features(acceleratorslib PUBLIC cxx_std_17)
