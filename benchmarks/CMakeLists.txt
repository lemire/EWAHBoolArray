if(NOT MSVC)
  add_executable(benchmark benchmark.cpp)
  target_link_libraries(benchmark PUBLIC ewah)
endif()
