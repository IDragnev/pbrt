set(CAMERAS_HEADERS_DIR ${PROJECT_SOURCE_DIR}/include/pbrt/cameras)
set(CAMERAS_HEADER_FILES
  ${CAMERAS_HEADERS_DIR}/OrthographicCamera.hpp
)

set(CAMERAS_SOURCE_FILES
  OrthographicCamera.cpp
)

add_library(cameraslib
  STATIC
  ${CAMERAS_SOURCE_FILES}
  ${CAMERAS_HEADER_FILES}
)
target_link_libraries(cameraslib
  PRIVATE corelib
)
target_include_directories(cameraslib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(cameraslib PUBLIC cxx_std_20)
target_compile_options(cameraslib
 PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)