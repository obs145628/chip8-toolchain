set(ODB_LIBS_DIR "/home/obs/rep/odb/_build/lib")
set(ODB_LIBS ${ODB_LIBS_DIR}/libodb_server_capi.a ${ODB_LIBS_DIR}/libodb_server.a ${ODB_LIBS_DIR}/libodb_mess.a)

set(SRC
  vm_api.c
  debug.c
)
add_library(oc8_debug ${SRC})
target_link_libraries(oc8_debug oc8_emu oc8_bin ${ODB_LIBS} -lstdc++)


add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")

set(TEST_SRC
  test_main.cc
  test_debug.cc
  test_db_client.cc
  ${CMAKE_SOURCE_DIR}/tests/test_src.c
)
set(TEST_NAME utest_oc8debug.bin)
add_executable(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_SRC})
target_link_libraries(${TEST_NAME} oc8_debug oc8_ld oc8_as) 
add_dependencies(build-tests ${TEST_NAME})
