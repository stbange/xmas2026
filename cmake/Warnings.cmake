# Strict C17 and warnings for our own targets only (third-party code keeps
# its defaults; e.g. SDL needs GNU extensions on Emscripten).
function(tb_target_warnings target)
  set_target_properties(${target} PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /wd4100)
    if(TB_WERROR)
      target_compile_options(${target} PRIVATE /WX)
    endif()
    target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options(${target} PRIVATE
      -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes
      -Wpointer-arith -Wcast-align -Wundef -Wvla -Wno-unused-parameter)
    if(TB_WERROR)
      target_compile_options(${target} PRIVATE -Werror)
    endif()
  endif()
  if(TB_SANITIZE AND NOT MSVC)
    target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
    target_link_options(${target} PRIVATE -fsanitize=address,undefined)
  endif()
endfunction()
