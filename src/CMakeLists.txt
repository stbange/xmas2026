# Game code. Every library is unit-tested; only src/platform/entry (SDL
# callback forwarders) and the raw GL backend may be excluded from the
# coverage metric (< 5 % of game C code, checked by tools/ci/coverage_gate.py).

function(tb_add_library name)
  add_library(${name} STATIC ${ARGN})
  target_include_directories(${name} PUBLIC ${PROJECT_SOURCE_DIR}/src)
  tb_target_warnings(${name})
  tb_target_coverage(${name})
endfunction()

add_subdirectory(core)
add_subdirectory(sim)
add_subdirectory(app)
add_subdirectory(game)

if(TB_BUILD_GAME)
  add_subdirectory(platform)
endif()
