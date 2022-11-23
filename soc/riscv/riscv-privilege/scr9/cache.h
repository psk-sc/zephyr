/*
 * @file cache.h
 *
 * @brief Syntacore L1/L2 cache interface
 *
 * @author amr-sc
 *
 * Copyright (c) 2022 Syntacore. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCR_CACHE_H
#define SCR_CACHE_H

/******************************************************************************
 *                              Includes section
 */
#include <soc_common.h>
#include <zephyr/sys/util.h>

/******************************************************************************
 *                              Defines section
 */
/* cache control CSRs */
#define SCR_CSR_CACHE_GLBL 0xbd4
/* cache info CSRs */
#define SCR_CSR_CACHE_DSCR_L1 0xfc3

/* global cache's control bits */
#define CACHE_GLBL_L1I_EN (1 << 0)
#define CACHE_GLBL_L1D_EN (1 << 1)
#define CACHE_GLBL_L1I_INV (1 << 2)
#define CACHE_GLBL_L1D_INV (1 << 3)
#define CACHE_GLBL_ENABLE (CACHE_GLBL_L1I_EN | CACHE_GLBL_L1D_EN)
#define CACHE_GLBL_DISABLE 0
#define CACHE_GLBL_INV (CACHE_GLBL_L1I_INV | CACHE_GLBL_L1D_INV)

/* L2$ registers */
#define L2_CSR_VER_IDX   0
#define L2_CSR_DESCR_IDX 1
#define L2_CSR_EN_IDX    4
#define L2_CSR_FLUSH_IDX 5
#define L2_CSR_INV_IDX   6

#define L2_CSR_VER_OFFS   (L2_CSR_VER_IDX * 4)
#define L2_CSR_DESCR_OFFS (L2_CSR_DESCR_IDX * 4)
#define L2_CSR_EN_OFFS    (L2_CSR_EN_IDX * 4)
#define L2_CSR_FLUSH_OFFS (L2_CSR_FLUSH_IDX * 4)
#define L2_CSR_INV_OFFS   (L2_CSR_INV_IDX * 4)

#define L2_CSR_VER   (PLF_L2CTL_BASE + L2_CSR_VER_OFFS)
#define L2_CSR_DESCR (PLF_L2CTL_BASE + L2_CSR_DESCR_OFFS)
#define L2_CSR_EN    (PLF_L2CTL_BASE + L2_CSR_FLUSH_OFFS)
#define L2_CSR_FLUSH (PLF_L2CTL_BASE + L2_CSR_FLUSH_OFFS)
#define L2_CSR_INV   (PLF_L2CTL_BASE + L2_CSR_INV_OFFS)

#define L2_CSR_DESCR_OFFS_BANKS      (16)
#define L2_CSR_DESCR_OFFS_WAYS       (0)
#define L2_CSR_DESCR_OFFS_LINESZ_LG2 (4)
#define L2_CSR_DESCR_OFFS_LINES_LG2  (8)
#define L2_CSR_DESCR_OFFS_CORES      (28)
#define L2_CSR_DESCR_OFFS_TYPE       (13)

#define L2_CSR_DESCR_MASK_BANKS      (0xf)
#define L2_CSR_DESCR_MASK_WAYS       (0x7)
#define L2_CSR_DESCR_MASK_LINESZ_LG2 (0xf)
#define L2_CSR_DESCR_MASK_LINES_LG2  (0x1f)
#define L2_CSR_DESCR_MASK_CORES      (0xf)
#define L2_CSR_DESCR_MASK_TYPE       (0x7)

#define SCR_L2_DESCR_BANKS(val)		(((val >> L2_CSR_DESCR_OFFS_BANKS) & \
						L2_CSR_DESCR_MASK_BANKS) + 1)
#define SCR_L2_DESCR_WAYS(val)		(1UL << ((val >> L2_CSR_DESCR_OFFS_WAYS) & \
						L2_CSR_DESCR_MASK_WAYS))
#define SCR_L2_DESCR_LINESZ(val)	(1UL << ((val >> L2_CSR_DESCR_OFFS_LINESZ_LG2) & \
						L2_CSR_DESCR_MASK_LINESZ_LG2))
#define SCR_L2_DESCR_LINES(val)		(1UL << ((val >> L2_CSR_DESCR_OFFS_LINES_LG2) & \
						L2_CSR_DESCR_MASK_LINES_LG2))
#define SCR_L2_DESCR_CORES(val)		(((val >> L2_CSR_DESCR_OFFS_CORES) & \
						L2_CSR_DESCR_MASK_CORES) + 1)

/******************************************************************************
 *                              Prototypes section
 */

/**
 * @brief Caches initialization
 */
void scr_cache_init(void);

/**
 * @brief Checks if L2 cache is enabled
 *
 * @return true - if L2 cache is enabled; false - otherwise
 */
bool scr_cache_l2_is_enabled(void);

/**
 * @brief Enables L2 cache
 */
void scr_cache_l2_enable(void);

/**
 * @brief Disables L2 cache
 */
void scr_cache_l2_disable(void);

/******************************************************************************
 *                              Implementation section
 */

#if IS_ENABLED(CONFIG_SCR_L1_CACHE)
/**
 * @brief Checks if L1 cache is available
 *
 * @return true - if L1 cache is available; false - otherwise
 */
static inline bool scr_cache_l1_available(void)
{
    return csr_read(SCR_CSR_CACHE_DSCR_L1) != 0;
}

/**
 * @brief Setup L1 cache global policy
 *
 * @param [in] ctrl_val - MEM_CTRL_GLOBAL register value
 */
static inline void scr_cache_l1_ctrl(unsigned long ctrl_val)
{
    if (scr_cache_l1_available()) {
    	csr_write(SCR_CSR_CACHE_GLBL, ctrl_val);
        ifence();
    }
}

/**
 * @brief Checks if L1 cache is enabled
 *
 * @return true - if L1 cache is enabled; false - otherwise
 */
static inline bool scr_cache_l1_enabled(void)
{
    if (!scr_cache_l1_available())
        return false;
    return csr_read(SCR_CSR_CACHE_GLBL) & (CACHE_GLBL_L1I_EN | CACHE_GLBL_L1D_EN);
}

/**
 * @brief Enables L1 cache
 */
static inline void scr_cache_l1_enable(void)
{
    scr_cache_l1_ctrl(CACHE_GLBL_ENABLE);
}

/**
 * @brief Disable L1 cache
 */
static inline void scr_cache_l1_disable(void)
{
    scr_cache_l1_ctrl(CACHE_GLBL_DISABLE | CACHE_GLBL_INV);

    while (csr_read(SCR_CSR_CACHE_GLBL) & (CACHE_GLBL_INV))
    	;
}

#endif /* IS_ENABLED(CONFIG_SCR_L1_CACHE) */

#endif /* SCR_CACHE_H */
