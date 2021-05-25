file(GLOB_RECURSE example_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(example_file ${example_files})
  configure_file(${example_file} ${example_file} COPYONLY)
endforeach(example_file)

function(add_cpp_test TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/cpp)
    add_test(NAME ${TEST_NAME} COMMAND "${TEST_NAME}" "${CMAKE_CURRENT_BINARY_DIR}/data/" )
    target_link_libraries(${TEST_NAME} PUBLIC ewah)
    if(NOT MSVC)  
    target_compile_options(
      ${TEST_NAME} PUBLIC
      -Wall -Wextra -Weffc++ -Wsign-compare -Wshadow -Wwrite-strings
      -Wpointer-arith -Winit-self -Wconversion -Wno-sign-conversion
    )
    endif()
    
endfunction(add_cpp_test)

add_cpp_test(unit)





