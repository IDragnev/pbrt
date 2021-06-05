find_package(Threads)

set(PARALLEL_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/parallel)
set(PARALLEL_HEADERS
  ${PARALLEL_HEADERS_DIR}/Parallel.hpp
)

set(PARALLEL_SOURCE_FILES
  Parallel.cpp
)

add_library(
  parallel
  STATIC
  ${PARALLEL_SOURCE_FILES}
  ${PARALLEL_HEADERS}
)
target_link_libraries(
  parallel
  PRIVATE ${CMAKE_THREAD_LIBS_INIT}
  PRIVATE functional
)
target_include_directories(parallel PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(parallel PUBLIC cxx_std_20)
target_compile_options(parallel
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)