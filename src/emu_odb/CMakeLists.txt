set(ODB_LIBS_DIR "/home/obs/rep/odb/_build/lib")
set(ODB_LIBS ${ODB_LIBS_DIR}/libodb_server.a ${ODB_LIBS_DIR}/libodb_server_capi.a)

set(SRC
  vm_api.c
)
add_library(emu_odb ${SRC})
target_link_libraries(emu_odb oc8_emu oc8_bin ${ODB_LIBS})


set(TEST_SRC
  test_main.cc
  test_debug.cc
  ${CMAKE_SOURCE_DIR}/tests/test_src.c
)
set(TEST_NAME utest_emu_odb.bin)
add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries(${TEST_NAME} emu_odb oc8_ld oc8_as) 
add_dependencies(build-tests ${TEST_NAME})
