# Script mode: writes IN as a C byte array named SYMBOL to OUT.
file(READ ${IN} hex HEX)
string(LENGTH "${hex}" n)
math(EXPR len "${n} / 2")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," bytes "${hex}")
string(REGEX REPLACE "((0x..,){16})" "\\1\n    " bytes "${bytes}")
file(WRITE ${OUT} "/* Generated from ${IN}. */\n#include <stddef.h>\n"
  "extern const char ${SYMBOL}[];\nextern const size_t ${SYMBOL}_len;\n"
  "const char ${SYMBOL}[] = {\n    ${bytes}0x00};\nconst size_t ${SYMBOL}_len = ${len};\n")
