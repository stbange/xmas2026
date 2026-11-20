# tb_embed_text(<target> <symbol> <input>): compiles the text file <input>
# into <target> as `const char <symbol>[]` and `const size_t <symbol>_len`.
# Regenerated whenever the input changes.
function(tb_embed_text target symbol input)
  set(out ${CMAKE_CURRENT_BINARY_DIR}/embed_${symbol}.c)
  add_custom_command(
    OUTPUT ${out}
    COMMAND ${CMAKE_COMMAND} -DIN=${input} -DOUT=${out} -DSYMBOL=${symbol}
            -P ${PROJECT_SOURCE_DIR}/cmake/EmbedRun.cmake
    DEPENDS ${input} ${PROJECT_SOURCE_DIR}/cmake/EmbedRun.cmake
    COMMENT "Embedding ${input}")
  target_sources(${target} PRIVATE ${out})
endfunction()
