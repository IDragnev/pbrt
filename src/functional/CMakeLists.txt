add_library(functional INTERFACE)
target_include_directories(functional INTERFACE ${PROJECT_SOURCE_DIR}/include/pbrt)
target_compile_features(functional INTERFACE cxx_std_17)