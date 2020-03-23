set(SRC
  main.c
)
add_executable(oc8-emu ${SRC})
target_link_libraries(oc8-emu oc8_is oc8_emu)
