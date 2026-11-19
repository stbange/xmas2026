# Platform glue. tb_platform is unit-tested (offscreen/X11 under Xvfb);
# only entry/ (SDL callback forwarders) is excluded from coverage.
tb_require(sdl3)

tb_add_library(tb_platform
  fs.c
  host.c
  storage.c
  window.c
)
target_link_libraries(tb_platform PUBLIC tb_game tb_app tb_core tb::sdl3)
if(TB_ALLOW_DATE_OVERRIDE)
  target_compile_definitions(tb_platform PRIVATE TB_ALLOW_DATE_OVERRIDE=1)
endif()

add_executable(advent_of_lan entry/main.c)
target_link_libraries(advent_of_lan PRIVATE tb_platform)
tb_target_warnings(advent_of_lan)

if(EMSCRIPTEN)
  # Deployable files only: build/<preset>/web-dist is what CI publishes to gh-pages.
  set_target_properties(advent_of_lan PROPERTIES OUTPUT_NAME "index" SUFFIX ".html"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/web-dist)
  target_link_options(advent_of_lan PRIVATE
    -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
    -sALLOW_MEMORY_GROWTH=1
    -sENVIRONMENT=web
    -sFORCE_FILESYSTEM=1
    -lidbfs.js
    --shell-file ${PROJECT_SOURCE_DIR}/web/shell.html)
  set_property(TARGET advent_of_lan APPEND PROPERTY LINK_DEPENDS ${PROJECT_SOURCE_DIR}/web/shell.html)
  # The shell shows the build id; substitute it at configure time.
  configure_file(${PROJECT_SOURCE_DIR}/web/build-info.js.in ${CMAKE_BINARY_DIR}/web/build-info.js @ONLY)
  add_custom_command(TARGET advent_of_lan POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/web/build-info.js $<TARGET_FILE_DIR:advent_of_lan>/build-info.js
    COMMAND ${CMAKE_COMMAND} -E touch $<TARGET_FILE_DIR:advent_of_lan>/.nojekyll)
else()
  # Game test: headless smoke run (deterministic, faster than real time).
  if(TB_BUILD_TESTS)
    add_test(NAME game.headless_smoke COMMAND advent_of_lan --headless --ticks 600
      --data-dir ${CMAKE_BINARY_DIR}/test-tmp/headless-smoke)
    set_tests_properties(game.headless_smoke PROPERTIES PASS_REGULAR_EXPRESSION "ticks=600 checksum=[0-9a-f]+" TIMEOUT 30)
  endif()
endif()
