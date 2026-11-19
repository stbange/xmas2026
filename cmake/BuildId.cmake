# Short git hash of the build, shown on the start screen and in the web shell.
set(TB_BUILD_ID "$ENV{TB_BUILD_ID}")
if(TB_BUILD_ID STREQUAL "")
  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" rev-parse --short=8 HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE TB_BUILD_ID
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET)
  endif()
endif()
if(TB_BUILD_ID STREQUAL "")
  set(TB_BUILD_ID "dev")
endif()
string(SUBSTRING "${TB_BUILD_ID}" 0 8 TB_BUILD_ID)
message(STATUS "Advent of LAN build id: ${TB_BUILD_ID}")
