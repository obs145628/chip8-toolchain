set(SRC
  main.c
  args-parser.c
)
add_executable(oc8-as ${SRC})
target_link_libraries(oc8-as oc8_as)
