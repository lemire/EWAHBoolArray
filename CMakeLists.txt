cmake_minimum_required(VERSION 2.8)
set(CMAKE_MACOSX_RPATH OFF)
if (NOT CMAKE_BUILD_TYPE)
                message(STATUS "No build type selected, default to Release")
                set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

project(EWAHBoolArray)


include_directories(headers)
install(DIRECTORY headers DESTINATION include)

enable_testing()
include(CTest)

add_subdirectory(src)


