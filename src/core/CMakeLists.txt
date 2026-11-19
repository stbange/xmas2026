tb_add_library(tb_core
  alloc.c
  arena.c
  dynarr.c
  hash.c
  hashmap.c
  log.c
  mathx.c
  rng.c
  serial.c
  timestep.c
  version.c
)
target_compile_definitions(tb_core PRIVATE
  TB_BUILD_ID="${TB_BUILD_ID}"
  TB_VERSION_STRING="${PROJECT_VERSION}")
if(UNIX AND NOT EMSCRIPTEN)
  target_link_libraries(tb_core PUBLIC m)
endif()
