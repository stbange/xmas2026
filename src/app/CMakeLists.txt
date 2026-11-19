tb_add_library(tb_app
  app.c
  args.c
  controls.c
  hud.c
  world_view.c
)
target_link_libraries(tb_app PUBLIC tb_render tb_game tb_sim tb_core)
