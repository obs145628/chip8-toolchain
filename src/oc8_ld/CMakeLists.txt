add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")

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
add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SRC})
add_dependencies(${TEST_NAME} oc8-as oc8-bin2rom oc8-ld oc8-rom2bin)
target_link_libraries(${TEST_NAME} oc8_as oc8_emu oc8_is oc8_ld) 
add_dependencies(build-tests ${TEST_NAME})
