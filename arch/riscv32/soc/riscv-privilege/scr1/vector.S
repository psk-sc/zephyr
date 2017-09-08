/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 * Copyright (C) Syntacore 2017. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <toolchain.h>

/* imports */
GTEXT(__start)
GTEXT(__irq_wrapper)

SECTION_FUNC(vectors, vinit)
	.option norvc;
machine_trap_entry:
    j do_irq_wrapper
1:
    j 1b
do_irq_wrapper:
    tail __irq_wrapper
    .align 6
