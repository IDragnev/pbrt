check_cxx_source_compiles(
  "
  #include <malloc.h>
  int main() { void* p = _aligned_malloc(1024, 32); }
  "
  HAS_ALIGNED_MALLOC)

check_cxx_source_compiles(
  "
  #include <stdlib.h>
  int main() {
      void *ptr;
      posix_memalign(&ptr, 32, 1024);
  }
  "
  HAS_POSIX_MEMALIGN)

check_cxx_source_compiles(
  "
  #include <malloc.h>
  int main() { void *p = memalign(32, 1024); }
  "
  HAS_MEMALIGN)

set(PBRT_MEMORY_SOURCE_FILES
  Memory.cpp
  MemoryArena.cpp
)

set(PBRT_MEMORY_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/memory)
set(PBRT_MEMORY_HEADERS
  ${PBRT_MEMORY_HEADERS_DIR}/Memory.hpp
  ${PBRT_MEMORY_HEADERS_DIR}/MemoryArena.hpp
  ${PBRT_MEMORY_HEADERS_DIR}/BlockedUVArray.hpp
  ${PBRT_MEMORY_HEADERS_DIR}/BlockedUVArrayImpl.hpp
)

add_library(
  memory 
  STATIC
  ${PBRT_MEMORY_SOURCE_FILES}
  ${PBRT_MEMORY_HEADERS}
)
target_include_directories(memory PUBLIC ${PROJECT_SOURCE_DIR}/include/pbrt)
target_compile_features(memory PUBLIC cxx_std_17)
target_compile_options(memory
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)

if(HAS_ALIGNED_MALLOC)
  target_compile_definitions(memory PRIVATE PBRT_HAS_ALIGNED_MALLOC)
elseif(HAS_POSIX_MEMALIGN)
  target_compile_definitions(memory PRIVATE PBRT_HAS_POSIX_MEMALIGN)
elseif(HAS_MEMALIGN)
  target_compile_definitions(memory PRIVATE PBRT_HAS_MEMALIGN)
else()
  message(SEND_ERROR "Unable to find a way to allocate aligned memory")
endif()