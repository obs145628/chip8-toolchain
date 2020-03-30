set(SRC
  file.c
)
add_library(oc8_bin ${SRC})
#target_link_libraries(oc8_as oc8_is oc8_smap)

#set(TEST_SRC
#  test_main.cc
#  test_parser.cc
#  test_sfile.cc
#  test_stream.cc
#)
#set(TEST_NAME utest_oc8as.bin)
#add_executable(${TEST_NAME} ${TEST_SRC})
#target_link_libraries(${TEST_NAME} oc8_as) 
#add_dependencies(build-tests ${TEST_NAME})
