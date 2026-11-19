/* SPDX-License-Identifier: MIT */
#include "render/gl/gl_api.h"

#include <stddef.h>
#include <string.h>

#define TB_GL_NAME(ret, name, args)   "gl" #name,
#define TB_GL_OFFSET(ret, name, args) offsetof(tb_gl, name),
static const char *const k_names[] = {TB_GL_FUNCS(TB_GL_NAME)};
static const size_t k_offsets[] = {TB_GL_FUNCS(TB_GL_OFFSET)};
#undef TB_GL_NAME
#undef TB_GL_OFFSET

int tb_gl_load(tb_gl *gl, tb_gl_loader loader, const char **missing)
{
    memset(gl, 0, sizeof(*gl));
    *missing = NULL;
    for (size_t i = 0; i < sizeof(k_names) / sizeof(k_names[0]); i++) {
        void *p = loader(k_names[i]);
        memcpy((char *)gl + k_offsets[i], &p, sizeof(p));
        if (!p && !*missing) {
            *missing = k_names[i];
        }
    }
    return *missing == NULL;
}
