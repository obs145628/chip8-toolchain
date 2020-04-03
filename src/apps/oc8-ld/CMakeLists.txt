set(SRC
  main.c
)
add_executable(oc8-ld ${SRC})
target_link_libraries(oc8-ld args_parser oc8_ld)
