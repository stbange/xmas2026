tb_add_library(tb_sim
  combat.c
  geometry.c
  inventory.c
  movement.c
  replay.c
  sim.c
  weapons.c
)
target_link_libraries(tb_sim PUBLIC tb_core)
