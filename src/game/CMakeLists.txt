tb_require(cjson)
tb_add_library(tb_game
  calendar.c
  hall.c
  levels.c
  save.c
  settings.c
)
target_link_libraries(tb_game PUBLIC tb_sim tb_core PRIVATE tb::cjson)
