cmake_minimum_required(VERSION 3.25)

project(AdventOfLAN
  VERSION 0.1.0
  DESCRIPTION "Advent of LAN - 24 portals to 2001"
  LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TB_BUILD_TESTS "Build unit tests" ON)
option(TB_BUILD_GAME "Build the game executable (needs SDL3)" ON)
option(TB_COVERAGE "Instrument game code for coverage (GCC/Clang)" OFF)
option(TB_SANITIZE "Build with AddressSanitizer + UBSan" OFF)
option(TB_WERROR "Treat warnings as errors" ON)
option(TB_PLAYTEST_BUILD "Playtest build: honour --playtest/?playtest=1 and --date/?date=" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Warnings)
include(Coverage)
include(BuildId)
include(Dependencies)

if(EMSCRIPTEN)
  set(TB_PLATFORM_WEB ON)
endif()

if(TB_BUILD_TESTS AND NOT EMSCRIPTEN)
  enable_testing()
endif()

add_subdirectory(src)

if(TB_BUILD_TESTS AND NOT EMSCRIPTEN)
  add_subdirectory(tests)
endif()
