set(FILTERS_SOURCE_FILES
  Box.cpp
  Triangle.cpp
  Gaussian.cpp
)

set(FILTERS_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/filters)
set(FILTERS_HEADER_FILES
  ${FILTERS_HEADERS_DIR}/Box.hpp
  ${FILTERS_HEADERS_DIR}/Triangle.hpp
  ${FILTERS_HEADERS_DIR}/Gaussian.hpp
)

add_library(
  filters
  STATIC
  ${FILTERS_HEADER_FILES}
  ${FILTERS_SOURCE_FILES}
)
target_link_libraries(filters
  PRIVATE corelib
)
target_include_directories(filters PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(filters PUBLIC cxx_std_20)
target_compile_options(filters PRIVATE ${PBRT_TARGET_WARNING_FLAGS})