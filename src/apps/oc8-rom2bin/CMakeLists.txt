set(SRC
  main.c
)
add_executable(oc8-rom2bin ${SRC})
target_link_libraries(oc8-rom2bin args_parser oc8_bin)
