tb_require(cjson)
tb_add_library(tb_game
  calendar.c
  save.c
  settings.c
)
target_link_libraries(tb_game PUBLIC tb_app tb_core PRIVATE tb::cjson)
