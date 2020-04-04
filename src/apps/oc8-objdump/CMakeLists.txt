set(SRC
  main.c
)
add_executable(oc8-objdump ${SRC})
target_link_libraries(oc8-objdump args_parser oc8_bin)
