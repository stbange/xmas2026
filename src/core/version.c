/* SPDX-License-Identifier: MIT */
#include "core/version.h"

#ifndef TB_BUILD_ID
#define TB_BUILD_ID "dev"
#endif
#ifndef TB_VERSION_STRING
#define TB_VERSION_STRING "0.0.0"
#endif

const char *tb_version(void)
{
    return TB_VERSION_STRING;
}

const char *tb_build_id(void)
{
    return TB_BUILD_ID;
}

const char *tb_version_banner(void)
{
    return "Advent of LAN " TB_VERSION_STRING " (" TB_BUILD_ID ")";
}
