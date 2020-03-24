set(SRC
  cpu.c
  exec_ins.c
  input.c
  mem.c
  screen.c
)
add_library(oc8_emu ${SRC})
target_link_libraries(oc8_emu oc8_is)


set(TEST_SRC
  test_main.cc
  test_ins.cc
  test_timer.cc
)
set(TEST_NAME utest_oc8emu.bin)
add_executable(${TEST_NAME} ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_emu) 
add_dependencies(build-tests ${TEST_NAME})
