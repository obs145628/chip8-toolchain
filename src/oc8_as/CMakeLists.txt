set(SRC
  as.c
  parser.c
  printer.c
  sfile.c
  stream.c
)
add_library(oc8_as ${SRC})
target_link_libraries(oc8_as oc8_bin oc8_is oc8_smap)

set(TEST_SRC
  test_main.cc
  test_as.cc
  test_parser.cc
  test_sfile.cc
  test_stream.cc
  ${CMAKE_SOURCE_DIR}/tests/test_src.c
)
set(TEST_NAME utest_oc8as.bin)
add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_as) 
add_dependencies(build-tests ${TEST_NAME})
