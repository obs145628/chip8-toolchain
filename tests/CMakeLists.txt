add_custom_target(build-tests)

add_custom_target(check
  COMMAND
  /home/obs/lun/obdev/bin/obdev ts run ${CMAKE_SOURCE_DIR}
  --out ${CMAKE_BINARY_DIR}/obts
  DEPENDS build-tests  
)
