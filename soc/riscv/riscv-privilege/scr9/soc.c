/*
 * @file soc.c
 *
 * @brief Syntacore SoC initialization
 *
 * @author amr-sc
 *
 * Copyright (c) 2022 Syntacore. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *                              Includes section
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/arch/cpu.h>
#include <soc.h>
#include "mpu.h"
#include "cache.h"

/******************************************************************************
 *                              Static variables section
 */
#if IS_ENABLED(CONFIG_SCR_MPU)
static const scr_mem_region_info mem_regions[] = {
    PLF_MEM_MAP,
    {0, 0, 0, 0}
};
#endif /* CONFIG_SCR_MPU */

/******************************************************************************
 *                              Implementation section
 */
/**
 * @brief Do initialization proccess at boot time.
 *
 * @param [in] arg - device context
 *
 * @return 0
 */
static int riscv64_scr9_init(const struct device *arg)
{
    /* Early MPU initialization - bring MPU to known state */
    if (IS_ENABLED(CONFIG_SCR_MPU))
    	scr_mpu_reset_init();

    /* Bring-up caches */
    if (IS_ENABLED(CONFIG_SCR_L1_CACHE) || IS_ENABLED(CONFIG_SCR_L2_CACHE))
    	scr_cache_init();

    /* Final MPU initialization */
    if (IS_ENABLED(CONFIG_SCR_MPU))
    	scr_mpu_setup(mem_regions);

    return 0;
}

SYS_INIT(riscv64_scr9_init, EARLY, 0);
