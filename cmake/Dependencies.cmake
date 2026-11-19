# All third-party dependencies, pinned to exact tags/commits.
# Declared here; a target pulls one in with tb_require(<name>).
# Licences: SDL3 zlib, cglm MIT, cgltf MIT, stb MIT/PD, Lua MIT,
# cJSON MIT, ENet MIT, Unity MIT (see CREDITS.md).
include(FetchContent)
set(FETCHCONTENT_QUIET ON)

FetchContent_Declare(sdl3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.4.18
  GIT_SHALLOW TRUE)
FetchContent_Declare(cglm
  GIT_REPOSITORY https://github.com/recp/cglm.git
  GIT_TAG v0.9.7
  GIT_SHALLOW TRUE)
FetchContent_Declare(cgltf
  GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
  GIT_TAG v1.15
  GIT_SHALLOW TRUE)
FetchContent_Declare(stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG 9600522de53973336fa033a289858a4354cbb04c)
FetchContent_Declare(lua
  GIT_REPOSITORY https://github.com/lua/lua.git
  GIT_TAG v5.4.9
  GIT_SHALLOW TRUE)
FetchContent_Declare(cjson
  GIT_REPOSITORY https://github.com/DaveGamble/cJSON.git
  GIT_TAG v1.7.19
  GIT_SHALLOW TRUE)
FetchContent_Declare(enet
  GIT_REPOSITORY https://github.com/lsalzman/enet.git
  GIT_TAG v1.3.18
  GIT_SHALLOW TRUE)
FetchContent_Declare(unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.6.1
  GIT_SHALLOW TRUE)

# tb_require(name): make a declared dependency available and define an
# imported/interface target tb::<name>.
function(tb_require name)
  if(TARGET tb::${name})
    return()
  endif()

  if(name STREQUAL "sdl3")
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(sdl3)
    add_library(tb::sdl3 ALIAS SDL3-static)

  elseif(name STREQUAL "unity")
    FetchContent_MakeAvailable(unity)
    target_compile_definitions(unity PUBLIC UNITY_INCLUDE_DOUBLE)
    add_library(tb::unity ALIAS unity)

  elseif(name STREQUAL "cglm")
    set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
    set(CGLM_STATIC ON CACHE BOOL "" FORCE)
    set(CGLM_USE_TEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(cglm)
    add_library(tb::cglm ALIAS cglm)

  elseif(name STREQUAL "cjson")
    FetchContent_Populate(cjson)
    add_library(tb_cjson STATIC ${cjson_SOURCE_DIR}/cJSON.c)
    target_include_directories(tb_cjson SYSTEM PUBLIC ${cjson_SOURCE_DIR})
    add_library(tb::cjson ALIAS tb_cjson)

  elseif(name STREQUAL "stb")
    FetchContent_Populate(stb)
    add_library(tb_stb INTERFACE)
    target_include_directories(tb_stb SYSTEM INTERFACE ${stb_SOURCE_DIR})
    add_library(tb::stb ALIAS tb_stb)

  elseif(name STREQUAL "cgltf")
    FetchContent_Populate(cgltf)
    add_library(tb_cgltf INTERFACE)
    target_include_directories(tb_cgltf SYSTEM INTERFACE ${cgltf_SOURCE_DIR})
    add_library(tb::cgltf ALIAS tb_cgltf)

  elseif(name STREQUAL "lua")
    FetchContent_Populate(lua)
    file(GLOB _lua_src ${lua_SOURCE_DIR}/*.c)
    list(FILTER _lua_src EXCLUDE REGEX "/(lua|luac|onelua|ltests)\\.c$")
    add_library(tb_lua STATIC ${_lua_src})
    target_include_directories(tb_lua SYSTEM PUBLIC ${lua_SOURCE_DIR})
    if(UNIX AND NOT EMSCRIPTEN)
      target_link_libraries(tb_lua PUBLIC m)
    endif()
    add_library(tb::lua ALIAS tb_lua)

  elseif(name STREQUAL "enet")
    FetchContent_MakeAvailable(enet)
    target_include_directories(enet SYSTEM PUBLIC ${enet_SOURCE_DIR}/include)
    add_library(tb::enet ALIAS enet)

  else()
    message(FATAL_ERROR "tb_require: unknown dependency '${name}'")
  endif()
endfunction()
