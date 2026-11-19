# Coverage instrumentation for game code (used by the linux-coverage preset).
# The report and the gates (>= 95 % lines, >= 90 % branches, platform
# exclusion < 5 %) are enforced by tools/ci/coverage_gate.py.
function(tb_target_coverage target)
  if(TB_COVERAGE)
    if(MSVC)
      message(FATAL_ERROR "TB_COVERAGE is only supported with GCC or Clang")
    endif()
    target_compile_options(${target} PRIVATE --coverage -O0 -g)
    target_link_options(${target} PUBLIC --coverage)
  endif()
endfunction()
