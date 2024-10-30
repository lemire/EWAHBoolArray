

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# We compile tools, tests, etc. with C++ 11. Override yourself if you need on a target.
set(EWAH_CXX_STANDARD 11 CACHE STRING "the C++ standard to use")

set(CMAKE_CXX_STANDARD ${EWAH_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
