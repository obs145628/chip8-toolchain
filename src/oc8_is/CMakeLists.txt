set(SRC
  ins.c
)
add_library(oc8_is ${SRC})


set(TEST_SRC
  test_main.cc
  test_encode.cc
  test_ins.cc
)
set(TEST_NAME utest_oc8is.bin)
add_executable(${TEST_NAME} ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_is) 
add_dependencies(build-tests ${TEST_NAME})
