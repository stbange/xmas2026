# One test executable per module.
set(TB_TEST_SUPPORT ${PROJECT_SOURCE_DIR}/tests/support/test_alloc.c)

foreach(m version alloc arena dynarr hash hashmap log mathx rng serial timestep)
  tb_add_unit_test(${m} LIBS tb_core SOURCES ${TB_TEST_SUPPORT})
endforeach()

foreach(m args app)
  tb_add_unit_test(${m} LIBS tb_app SOURCES ${TB_TEST_SUPPORT})
endforeach()

if(TB_BUILD_GAME)
  foreach(m platform_host platform_fs)
    tb_add_unit_test(${m} LIBS tb_platform)
  endforeach()
endif()
