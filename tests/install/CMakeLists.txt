
cmake_minimum_required(VERSION 3.15)

project(test_ewah_install VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ewah REQUIRED)

file(WRITE main.cpp "
#include \"ewah/ewah.h\"
#include <stdlib.h>
#include <time.h>

using namespace ewah;

int main() {
  EWAHBoolArray<uint32_t> bitmap;
  return 0;
}")

add_executable(reproc main.cpp)
target_link_libraries(reproc PUBLIC ewah::ewah)
