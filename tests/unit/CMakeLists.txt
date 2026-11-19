# One test executable per module.
set(TB_TEST_SUPPORT ${PROJECT_SOURCE_DIR}/tests/support/test_alloc.c)

foreach(m version alloc arena dynarr hash hashmap log mathx rng serial timestep)
  tb_add_unit_test(${m} LIBS tb_core SOURCES ${TB_TEST_SUPPORT})
endforeach()

foreach(m args app controls flow hud)
  tb_add_unit_test(${m} LIBS tb_app SOURCES ${TB_TEST_SUPPORT})
endforeach()

foreach(m geometry movement combat weapons inventory sim replay)
  tb_add_unit_test(${m} LIBS tb_sim SOURCES ${TB_TEST_SUPPORT})
endforeach()

tb_add_unit_test(render_cpu LIBS tb_render SOURCES ${TB_TEST_SUPPORT})

foreach(m calendar save settings hall)
  tb_add_unit_test(${m} LIBS tb_game)
endforeach()

if(TB_BUILD_GAME)
  tb_add_unit_test(renderer LIBS tb_render tb::sdl3)
  foreach(m platform_host platform_fs platform_storage)
    tb_add_unit_test(${m} LIBS tb_platform)
  endforeach()
endif()
