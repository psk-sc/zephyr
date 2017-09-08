/*
 * Copyright (C) Syntacore 2017. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file SoC configuration macros for the Syntacore SCR1 core
 */

#ifndef __RISCV32_SCR1_SOC_H_
#define __RISCV32_SCR1_SOC_H_

#include <soc_common.h>
#include <misc/util.h>

#define SCR_SYS_CLK_FREQ   CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC

#ifndef SCR_SYS_CLK_FREQ
# error SCR_SYS_CLK_FREQ  not defined
#endif

/* UART configuration */
#define SCR_UART0_PORT     CONFIG_SCR_UART_BASE

#define SCR_UART_RXD       (0x00) // receive data
#define SCR_UART_TXD       (0x00) // transmit data
#define SCR_UART_IER       (0x04) // interrupt enable register
#define SCR_UART_FCR       (0x08) // FIFO control register
#define SCR_UART_CONTROL   (0x0c) // control register
#define SCR_UART_MCR       (0x10) // modem control register
#define SCR_UART_STATUS    (0x14) // status register
#define SCR_UART_DIV_LO    (0x00) // baud rate divisor register, low
#define SCR_UART_DIV_HI    (0x04) // baud rate divisor register, low

// UART status register bits
#define SCR_UART_ST_TEMPTY (1 << 6) // tx empty
#define SCR_UART_ST_TRDY   (1 << 5) // tx not full
#define SCR_UART_ST_RRDY   (1 << 0) // rx not empty

#define SCR_UART_115200_CLK_DIVISOR ((SCR_SYS_CLK_FREQ / 115200 + 7) / 16)

/* Timer configuration */
#define RISCV_MTIME_BASE             CONFIG_RISCV_MTIME_BASE

#define SCR_RTC_TIMEBASE    1000000

#define SCR_RTC_CTL       (RISCV_MTIME_BASE+0)
#define SCR_RTC_DIVIDER   (RISCV_MTIME_BASE+4)
#define SCR_RTC_MTIME     (RISCV_MTIME_BASE+8)
#define SCR_RTC_MTIMEH    (RISCV_MTIME_BASE+12)
#define SCR_RTC_MTIMECMP  (RISCV_MTIME_BASE+16)
#define SCR_RTC_MTIMECMPH (RISCV_MTIME_BASE+20)
#define SCR_RTC_CTL_EN (1 << 0)
#define SCR_RTC_CTL_INTERNAL_SRC (0 << 1)
#define SCR_RTC_CTL_EXTERNAL_SRC (1 << 1)
#define RTC_TIMEBASE_DIVISOR ((SCR_SYS_CLK_FREQ) / (SCR_RTC_TIMEBASE) - 1)
#define SCR_RTC_CTL_SRC SCR_RTC_CTL_INTERNAL_SRC

#define RISCV_RESET_VEC_ADDR         CONFIG_RISCV_RESET_VEC_ADDR
#define RISCV_MACHINE_TRAP_ADDR      CONFIG_RISCV_MACHINE_TRAP_ADDR

#define RISCV_RAM_BASE_ADDR          CONFIG_RISCV_RAM_BASE_ADDR
#define RISCV_RAM_SIZE               KB(CONFIG_RISCV_RAM_SIZE_KB)

#define RISCV_TCM_BASE_ADDR          CONFIG_RISCV_TCM_BASE_ADDR
#define RISCV_TCM_SIZE               KB(CONFIG_RISCV_TCM_SIZE_KB)

#endif /* __RISCV32_SCR1_SOC_H_ */
