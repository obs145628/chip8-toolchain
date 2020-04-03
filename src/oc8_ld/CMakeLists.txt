set(SRC
  linker.c
)
add_library(oc8_ld ${SRC})
target_link_libraries(oc8_ld oc8_bin oc8_is)

set(TEST_SRC
  test_main.cc
  test_linker.cc
  ${CMAKE_SOURCE_DIR}/tests/test_src.c
)
set(TEST_NAME utest_oc8ld.bin)
add_executable(${TEST_NAME} ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_as) 
add_dependencies(build-tests ${TEST_NAME})
