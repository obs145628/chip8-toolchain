set(SRC
  main.c
)
add_executable(oc8-bin2rom ${SRC})
target_link_libraries(oc8-bin2rom args_parser oc8_bin)
