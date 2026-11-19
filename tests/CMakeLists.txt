tb_require(unity)

# tb_add_unit_test(<module> LIBS <libs...>)
# Builds tests/unit/test_<module>.c into its own executable and registers it.
function(tb_add_unit_test module)
  cmake_parse_arguments(T "" "" "LIBS;SOURCES" ${ARGN})
  set(exe test_${module})
  add_executable(${exe} ${PROJECT_SOURCE_DIR}/tests/unit/test_${module}.c ${T_SOURCES})
  target_link_libraries(${exe} PRIVATE tb::unity ${T_LIBS})
  target_include_directories(${exe} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/tests)
  tb_target_warnings(${exe})
  if(MSVC)
    # Unity's hex assertion macros cast constants to narrower types.
    target_compile_options(${exe} PRIVATE /wd4310)
  endif()
  tb_target_coverage(${exe})
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test-tmp)
  target_compile_definitions(${exe} PRIVATE TB_TEST_TMP_DIR="${CMAKE_BINARY_DIR}/test-tmp")
  add_test(NAME unit.${module} COMMAND ${exe})
endfunction()

add_subdirectory(unit)
