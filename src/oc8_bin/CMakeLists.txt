add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")

set(SRC
  bin_reader.c
  bin_writer.c
  file.c
  format.c
  printer.c
)
add_library(oc8_bin ${SRC})
target_link_libraries(oc8_bin oc8_is oc8_smap)

set(TEST_SRC
  test_main.cc
  test_format.cc
  test_objdump.cc
  ${CMAKE_SOURCE_DIR}/tests/test_src.c
)
set(TEST_NAME utest_oc8bin.bin)
add_executable(${TEST_NAME} ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_bin oc8_as)
add_dependencies(build-tests ${TEST_NAME})


# @TODO remove all test binaries from all
