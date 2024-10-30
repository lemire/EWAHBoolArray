cmake_minimum_required(VERSION 3.5)

project(EWAHBoolArray
  DESCRIPTION "A compressed bitmap class in C++"
  LANGUAGES CXX
  VERSION 0.8.0
)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/ewah-flags.cmake)


add_library(ewah INTERFACE)

target_include_directories(
  ewah
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(ewah::ewah ALIAS ewah)

enable_testing()

add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(benchmarks)


install(
    TARGETS ewah
    EXPORT ewahTargets
    RUNTIME COMPONENT ewah_Runtime
    LIBRARY COMPONENT ewah_Runtime
    ARCHIVE COMPONENT ewah_Development
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
configure_file(cmake/ewah-config.cmake.in ewah-config.cmake @ONLY)


write_basic_package_version_file(
    ewah-config-version.cmake
    COMPATIBILITY SameMinorVersion
)
set(
    EWAH_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/ewah"
    CACHE STRING "CMake package config location relative to the install prefix"
)
mark_as_advanced(EWAH_INSTALL_CMAKEDIR)

install(
    FILES
    "${PROJECT_BINARY_DIR}/ewah-config.cmake"
    "${PROJECT_BINARY_DIR}/ewah-config-version.cmake"
    DESTINATION "${EWAH_INSTALL_CMAKEDIR}"
    COMPONENT ewah_Development
)

message(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")

message(STATUS "CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}")
# pkg-config
include(cmake/JoinPaths.cmake)
join_paths(PKGCONFIG_INCLUDEDIR "\${prefix}" "${CMAKE_INSTALL_INCLUDEDIR}")
join_paths(PKGCONFIG_LIBDIR "\${prefix}" "${CMAKE_INSTALL_LIBDIR}")

configure_file("ewah.pc.in" "ewah.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/ewah.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/ewah" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(TARGETS ewah
          EXPORT ${PROJECT_NAME}-targets
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
    EXPORT ewahTargets
    NAMESPACE ewah::
    DESTINATION "${EWAH_INSTALL_CMAKEDIR}"
    COMPONENT example_Development
)

#
# CPack
#
if(is_top_project)
  set(CPACK_PACKAGE_VENDOR "Daniel Lemire")
  set(CPACK_PACKAGE_CONTACT "daniel@lemire.me")
  set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
  set(CPACK_RPM_PACKAGE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
  set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
  set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
  include(CPack)
endif()
