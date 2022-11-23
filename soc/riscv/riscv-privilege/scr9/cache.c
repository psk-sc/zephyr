/*
 * @file cache.c
 *
 * @brief Syntacore L1/L2 cache support
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
#include "cache.h"

#if IS_ENABLED(CONFIG_SCR_L1_CACHE) || IS_ENABLED(CONFIG_SCR_L2_CACHE)

/******************************************************************************
 *                              Implementation section
 */

#if IS_ENABLED(CONFIG_SCR_L2_CACHE)

/**
 * @brief Enables L2 cache
 */
void scr_cache_l2_enable(void)
{
	/* init L2$: disable, confirm  state, invalidate, confirm, enable, confirm */
	volatile uint32_t *l2ctl = (uint32_t*)PLF_L2CTL_BASE;

	if (!l2ctl[L2_CSR_VER_IDX])
		return; /* cache not exists or not supported */

	uint32_t l2desc0 = l2ctl[L2_CSR_DESCR_IDX];
	uint32_t cbmask = (1 << SCR_L2_DESCR_BANKS(l2desc0)) - 1;

	/* disable L2$ */
	l2ctl[L2_CSR_EN_IDX] = 0;
	/* confirm state */
	while (l2ctl[L2_CSR_EN_IDX]);
	/* invalidate */
	l2ctl[L2_CSR_INV_IDX] = cbmask;
	/* confirm state */
	while (l2ctl[L2_CSR_INV_IDX]);
	/* enable */
	l2ctl[L2_CSR_EN_IDX] = cbmask;
	/* confirm state */
	while (l2ctl[L2_CSR_EN_IDX] != cbmask);
}

/**
 * @brief Disables L2 cache
 */
void scr_cache_l2_disable(void)
{
	volatile uint32_t *l2ctl = (uint32_t*)PLF_L2CTL_BASE;

	if (!l2ctl[L2_CSR_VER_IDX])
		return; /* cache not exists or not supported */

	/* disable */
	l2ctl[L2_CSR_EN_IDX] = 0;
	/* confirm state */
	while (l2ctl[L2_CSR_EN_IDX]);
	/* flush overall */
	l2ctl[L2_CSR_FLUSH_IDX] = ~0U;
	/* confirm state */
	while (l2ctl[L2_CSR_FLUSH_IDX]);
	/* invalidate */
	l2ctl[L2_CSR_INV_IDX] = ~0U;
	/* confirm state */
	while (l2ctl[L2_CSR_INV_IDX]);
}

/**
 * @brief Checks if L2 cache is enabled
 *
 * @return true - if L2 cache is enabled; false - otherwise
 */
bool scr_cache_l2_is_enabled(void)
{
	volatile uint32_t *l2ctl = (uint32_t*)PLF_L2CTL_BASE;

	return l2ctl[L2_CSR_VER_IDX] && l2ctl[L2_CSR_EN_IDX];
}

#endif /* IS_ENABLED(CONFIG_SCR_L2_CACHE) */

/**
 * @brief Caches initialization
 */
void scr_cache_init(void)
{
	if (IS_ENABLED(CONFIG_SCR_L2_CACHE))
		scr_cache_l2_enable();
	if (IS_ENABLED(CONFIG_SCR_L1_CACHE))
		scr_cache_l1_enable();
}

#endif /* IS_ENABLED(CONFIG_SCR_L1_CACHE) || IS_ENABLED(CONFIG_SCR_L2_CACHE) */
