/*
 * Copyright (c) 2017 Jean-Paul Etienne <fractalclone@gmail.com>
 * Copyright (c) 2017 Syntacore. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <device.h>
#include <system_timer.h>
#include <board.h>

typedef struct {
	u32_t val_low;
	u32_t val_high;
} riscv_machine_timer_t;

static volatile riscv_machine_timer_t *mtime =
	(riscv_machine_timer_t *)SCR_RTC_MTIME;
static volatile riscv_machine_timer_t *mtimecmp =
	(riscv_machine_timer_t *)SCR_RTC_MTIMECMP;

static ALWAYS_INLINE void riscv_machine_rearm_timer(void)
{
	u64_t rtc;
    u32_t hi1, hi0, lo;

	/* Disable timer interrupt */
	irq_disable(RISCV_MACHINE_TIMER_IRQ);

    do {
        hi0 = mtime->val_high;
        lo  = mtime->val_low;
        hi1 = mtime->val_high;
    } while (hi0 != hi1);

	rtc = lo;
	rtc |= ((u64_t)hi1 << 32);

	/*
	 * Rearm timer to generate an interrupt after
	 * sys_clock_hw_cycles_per_tick diveded on RTC_TIMEBASE_DIVISOR
	 */
	rtc += sys_clock_hw_cycles_per_tick / RTC_TIMEBASE_DIVISOR;

	mtimecmp->val_high = 0xffffffff;
	mtimecmp->val_low  = (u32_t)(rtc & 0xffffffff);
	mtimecmp->val_high = (u32_t)((rtc >> 32) & 0xffffffff);

	/* Enable timer interrupt */
	irq_enable(RISCV_MACHINE_TIMER_IRQ);
}

static void riscv_machine_timer_irq_handler(void *unused)
{
	ARG_UNUSED(unused);

	_sys_clock_tick_announce();

	/* Rearm timer */
	riscv_machine_rearm_timer();
}

#ifdef CONFIG_TICKLESS_IDLE
#error "Tickless idle not yet implemented for riscv-machine timer"
#endif

int _sys_clock_driver_init(struct device *device)
{
	ARG_UNUSED(device);

	IRQ_CONNECT(RISCV_MACHINE_TIMER_IRQ, 0,
		    riscv_machine_timer_irq_handler, NULL, 0);

    /* configure RTC timebase (divisor) and reset time counters */
    *(volatile uint32_t*)SCR_RTC_CTL = 0;
    *(volatile uint32_t*)SCR_RTC_MTIME = 0;
    *(volatile uint32_t*)SCR_RTC_MTIMEH = 0;
    *(volatile uint32_t*)SCR_RTC_MTIMECMPH = 0xffffffff;
    *(volatile uint32_t*)SCR_RTC_MTIMECMP = 0xffffffff;
    *(volatile uint32_t*)SCR_RTC_DIVIDER = RTC_TIMEBASE_DIVISOR;
    *(volatile uint32_t*)SCR_RTC_CTL = SCR_RTC_CTL_EN | SCR_RTC_CTL_SRC;

	riscv_machine_rearm_timer();

	return 0;
}

u32_t _timer_cycle_get_32(void)
{
	return mtime->val_low;
}
