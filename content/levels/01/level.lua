-- SPDX-License-Identifier: CC-BY-4.0
-- Day 1: Baublegum City. The level's story beats and the Golden Joystick
-- challenge "Interior Decorator" (use one of everything: breaker switch,
-- toilet, vending machine, arcade cabinet, projector button).
-- Callbacks get (name, value, player); see src/game/script.h.

local USABLE_KINDS = 5 -- switch, toilet, vending, arcade, button
local used = {}
local used_count = 0
arcade_plays = 0
boss_down = false

function on_start()
  tb.light(2, false) -- the stairwell is dark until the breaker is thrown
  tb.light(3, false) -- the parking deck alarm
  tb.objective("GET OFF THE ROOF")
  tb.say("NOEL: Crash landing on a mall roof. Classy. Find the breaker for that stairwell door.")
end

function on_used(name, kind, player)
  if not used[kind] then
    used[kind] = true
    used_count = used_count + 1
    if used_count == USABLE_KINDS then
      tb.challenge_done()
      tb.say("NOEL: You touched everything in this mall. Golden Joystick: Interior Decorator.")
    end
  end
  if name == "breaker" then
    tb.objective("HEAD DOWN INTO THE MALL")
    tb.say("NOEL: Power's back. Mind the pigs in uniform.")
  elseif name == "toilet" then
    tb.say("NOEL: Feel better? I'll pretend I didn't see that.")
  elseif name == "vending" then
    tb.say("NOEL: Cola. The breakfast of champions.")
  elseif name == "arcade" then
    arcade_plays = arcade_plays + 1
    if arcade_plays == 3 then
      tb.spawn_pickup("floppy", 50.5, 9.0, 0.5, 1)
      tb.say("NOEL: New high score! The machine coughed up a save disk.")
    elseif arcade_plays < 3 then
      tb.say("NOEL: Game over. Insert coin.")
    end
  elseif name == "projector" then
    tb.say("NOEL: Now showing: whatever is behind that screen.")
  end
end

function on_trigger(name, value, player)
  if name == "toystore" then
    tb.objective("FIND A WAY THROUGH THE TOY STORE")
    tb.say("NOEL: That Shrink-O-Matic display looks... functional. And that vent looks small.")
  elseif name == "quake" then
    tb.shake(150)
    tb.fire("bridge")
    tb.objective("CROSS THE ATRIUM")
    tb.say("NOEL: EARTHQUAKE! There goes the bridge. Grab that jetpack, hotshot.")
  elseif name == "deck" then
    tb.light(3, true)
    tb.spawn_enemy("battlelord", 186, 0, 0, "boss")
    tb.objective("DEFEAT THE BARGAIN-BIN BATTLELORD")
    tb.say("NOEL: Everything must go! Including you, apparently. Use the pillars!")
  end
end

function on_killed(name, kind, player)
  if name == "boss" then
    boss_down = true
    tb.light(3, false)
    tb.spawn_pickup("bauble", 186, 0, 1.0)
    tb.fire("exit_door")
    tb.objective("TAKE THE MEMORY BAUBLE AND LEAVE")
    tb.say("NOEL: Clearance sale over. That bauble is ours!")
  end
end

function on_pickup(name, kind, player)
  if kind == 5 then -- the Memory Bauble
    tb.say("NOEL: Memory Bauble 1 secured. The exit is open.")
  end
end
