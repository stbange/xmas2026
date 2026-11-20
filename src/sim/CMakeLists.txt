tb_add_library(tb_sim
  ai.c
  combat.c
  geometry.c
  inventory.c
  movement.c
  replay.c
  sim.c
  weapons.c
  world.c
)
target_link_libraries(tb_sim PUBLIC tb_core)
