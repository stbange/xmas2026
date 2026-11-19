# CPU-side rendering (pure, unit-tested) and the OpenGL backend (tested with
# a real GL context under Xvfb + Mesa in CI).
tb_add_library(tb_render
  camera.c
  mesh.c
  palette.c
  scene.c
  view.c
  gl/gl_api.c
  gl/renderer.c
)
target_link_libraries(tb_render PUBLIC tb_sim tb_core)
