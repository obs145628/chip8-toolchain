find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SRC
  main.c
  sdl-env.c
)
add_executable(oc8-emu ${SRC})
target_link_libraries(oc8-emu oc8_is oc8_emu ${SDL2_LIBRARIES} pthread)
