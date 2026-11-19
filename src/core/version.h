/* SPDX-License-Identifier: MIT */
#ifndef TB_CORE_VERSION_H
#define TB_CORE_VERSION_H

/* Game version, e.g. "0.1.0". */
const char *tb_version(void);

/* Short build id (git hash), e.g. "d0699863", or "dev". */
const char *tb_build_id(void);

/* Human-readable "Advent of LAN 0.1.0 (d0699863)". */
const char *tb_version_banner(void);

#endif
