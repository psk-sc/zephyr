/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 * Copyright (C) Syntacore 2017. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/toolchain.h>

/* imports */
GTEXT(__start)
GTEXT(_isr_wrapper)

/* exports */
GTEXT(machine_trap_entry)

SECTION_FUNC(vectors, vinit)
	.option norvc;
machine_trap_entry:
    tail _isr_wrapper
    .align 2
