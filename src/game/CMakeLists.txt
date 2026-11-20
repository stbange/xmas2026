tb_require(cjson)
tb_require(lua)
tb_add_library(tb_game
  bot.c
  calendar.c
  hall.c
  level.c
  levels.c
  levels/day01.c
  save.c
  script.c
  settings.c
)
target_link_libraries(tb_game PUBLIC tb_sim tb_core PRIVATE tb::cjson tb::lua)
tb_embed_text(tb_game tb_level01_lua ${PROJECT_SOURCE_DIR}/content/levels/01/level.lua)
