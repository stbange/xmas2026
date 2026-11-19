tb_add_library(tb_app
  app.c
  args.c
  controls.c
)
target_link_libraries(tb_app PUBLIC tb_sim tb_core)
