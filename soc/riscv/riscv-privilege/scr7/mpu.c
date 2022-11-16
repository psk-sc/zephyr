/*
 * @file mpu.c
 *
 * @brief Syntacore MPU support
 *
 * @author dz-sc
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
#include <zephyr/arch/riscv/csr.h>
#include <soc.h>
#include "mpu.h"
#include "cache.h"

#if IS_ENABLED(CONFIG_SCR_MPU)

/******************************************************************************
 *                              Implementation section
 */
/**
 * @brief Sets MPU memory region attributes
 *
 * @param [in] regn - region number
 * @param [in] attr - region attributes
 */
static inline void scr_mpu_region_update_attr(long regn, unsigned long attr)
{
	csr_write(SCR_CSR_MPU_SEL, regn);
	csr_write(SCR_CSR_MPU_CTRL, attr);
}

/**
 * @brief Gets MPU memory region attributes
 *
 * @param [in] regn - region number
 *
 * @return MPU memory region attributes
 */
static inline unsigned long scr_mpu_region_get_attr(long regn)
{
	csr_write(SCR_CSR_MPU_SEL, regn);
	return csr_read(SCR_CSR_MPU_CTRL);
}

/**
 * @brief Disables MPU memory region.
 *
 * @param [in] regn - region number
 */
static inline void scr_mpu_region_disable(long regn)
{
	scr_mpu_region_update_attr(regn, 0);
}

/**
 * @brief Active MPU memory region update.
 *		Region will not be unexpectedly disabled during the process
 *
 * @param [in] regn - region number
 * @param [in] base - region base address
 * @param [in] size - region size
 * @param [in] attr - region attributes
 */
static inline void scr_mpu_region_update(long regn, uintptr_t base, unsigned long size, unsigned long attr)
{
	csr_write(SCR_CSR_MPU_SEL, regn);
	csr_write(SCR_CSR_MPU_CTRL, attr);
	csr_write(SCR_CSR_MPU_ADDR, SCR_MPU_MK_ADDR(base));
	csr_write(SCR_CSR_MPU_MASK, SCR_MPU_MK_MASK(size));
}

/**
 * @brief MPU memory region configuration
 *
 * @param [in] regn - region number
 * @param [in] base - region base address
 * @param [in] size - region size
 * @param [in] attr - region attributes
 */
static inline void scr_mpu_region_setup(long regn, uintptr_t base, unsigned long size, unsigned long attr)
{
	csr_write(SCR_CSR_MPU_SEL, regn);
	csr_write(SCR_CSR_MPU_CTRL, 0);
	csr_write(SCR_CSR_MPU_ADDR, SCR_MPU_MK_ADDR(base));
	csr_write(SCR_CSR_MPU_MASK, SCR_MPU_MK_MASK(size));
	csr_write(SCR_CSR_MPU_CTRL, attr);
}

/**
 * @brief Early MPU initialization. Brings MPU to known state after possible
 *		warm reset
 */
void scr_mpu_reset_init(void)
{
	/* region0= NC,SO, global */
	scr_mpu_region_update(0, 0, 0, SCR_MPU_CTRL_MA | SCR_MPU_CTRL_MT_STRONG | SCR_MPU_CTRL_VALID);
	ifence();
	/* region1= MMCFG */
	scr_mpu_region_setup(1, PLF_MMCFG_BASE, PLF_MMCFG_SIZE, PLF_MMCFG_ATTR | SCR_MPU_CTRL_VALID);
	/* region2= MMIO */
	scr_mpu_region_setup(2, PLF_MMIO_BASE, PLF_MMIO_SIZE, PLF_MMIO_ATTR | SCR_MPU_CTRL_VALID);
	/* all other regions= disabled */
	for (unsigned rgn = 3; ; ++rgn) {
		csr_write(SCR_CSR_MPU_SEL, rgn);
		if (csr_read(SCR_CSR_MPU_SEL) == 0)
			break;
		csr_write(SCR_CSR_MPU_CTRL, 0);
	}
}

/**
 * @brief MPU configuration
 *
 * @param [in] mem_regions - array of memory region descriptors
 */
void scr_mpu_setup(const scr_mem_region_info *mem_regions)
{
	/* configure all regions */
	const scr_mem_region_info *rgn = mem_regions;
	unsigned rn = 1;
	while (rgn->size && rn < SCR_MPU_REGIONS) {
		if (rgn->attr) {
			scr_mpu_region_setup(rn, rgn->base, rgn->size, rgn->attr | SCR_MPU_CTRL_VALID);
			ifence();
			++rn;
		}
		++rgn;
	}

	if (rn > 1) {
		/* disable all unused regions, disable default region #0 */
		do {
			csr_write(SCR_CSR_MPU_SEL, rn++);
			csr_write(SCR_CSR_MPU_CTRL, 0);
			ifence();
		} while (rn < SCR_MPU_REGIONS);

		csr_write(SCR_CSR_MPU_SEL, 0);
		csr_write(SCR_CSR_MPU_CTRL, 0);
	}
}

#endif /* CONFIG_SCR_MPU */
