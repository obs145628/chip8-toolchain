set(SRC
  main.c
)
add_executable(oc8-as ${SRC})
target_link_libraries(oc8-as args_parser oc8_as)
