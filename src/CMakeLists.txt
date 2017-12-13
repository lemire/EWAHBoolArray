

if (CMAKE_VERSION VERSION_GREATER 2.8.10)
  function(add_cpp_test TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
    add_test(${TEST_NAME} ${TEST_NAME})
  endfunction(add_cpp_test)
else()
  function(add_cpp_test TEST_NAME)
    MESSAGE( STATUS "Your CMake version is too old for our C++ test script: " ${CMAKE_VERSION} )
  endfunction(add_cpp_test)
endif()

add_cpp_test(unit)

