set(ALL_APP_LIBS
  corelib
  shapeslib
)

add_executable(raytracer_exe main.cpp)
set_target_properties(raytracer_exe PROPERTIES OUTPUT_NAME raytracer)
target_link_libraries(raytracer_exe ${ALL_APP_LIBS})
target_compile_options(raytracer_exe
  PRIVATE ${PBRT_TARGET_WARNING_FLAGS}
)