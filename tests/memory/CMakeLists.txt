add_executable(memory_test
  main.cpp
  memory.cpp
  memoryArena.cpp
)
target_link_libraries(memory_test memory doctest)