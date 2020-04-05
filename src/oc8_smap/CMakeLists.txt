set(SRC
  oc8_smap.c
)
add_library(oc8_smap ${SRC})


set(TEST_SRC
  test_main.cc
  test_smap.cc
)
set(TEST_NAME utest_oc8smap.bin)
add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_smap) 
add_dependencies(build-tests ${TEST_NAME})
