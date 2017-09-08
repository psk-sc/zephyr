/*
 * Copyright (C) 2017 Syntacore. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <uart.h>
#include <sys_io.h>

#define DEV_CFG(dev)					\
	((const struct uart_device_config * const)	\
	 (dev)->config->config_info)

static inline void scr_uart_write(uintptr_t uart_base, unsigned reg, uint8_t val)
{
    *(volatile uint8_t *)(uart_base + reg) = val;
}

static inline uint8_t scr_uart_read(uintptr_t uart_base, unsigned reg)
{
    return *(volatile uint8_t *)(uart_base + reg);
}


static int uart_scr_init(struct device *dev)
{
    /* disable interrupts */
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_IER, 0);
    /* init MCR */
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_MCR, 0);
    /* setup baud rate */
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_CONTROL, 0x3 | 0x80);
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_DIV_LO, SCR_UART_115200_CLK_DIVISOR & 0xff);
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_DIV_HI, (SCR_UART_115200_CLK_DIVISOR >> 8) & 0xff);
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_CONTROL, 0x3);
    /* reset fifo and disable */
    scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_FCR, (1 << 1));

	return 0;
}

static inline int scr_uart_rx_ready(void)
{
    return scr_uart_read(SCR_UART0_PORT, SCR_UART_STATUS) & SCR_UART_ST_RRDY;
}

static inline int scr_uart_getch_nowait(void)
{
    if (scr_uart_rx_ready())
        return scr_uart_read(SCR_UART0_PORT, SCR_UART_RXD);
    return -1;
}

static inline int scr_uart_tx_ready(void)
{
    return scr_uart_read(SCR_UART0_PORT, SCR_UART_STATUS) & SCR_UART_ST_TRDY;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

static unsigned char uart_scr_poll_out(struct device *dev,
					      unsigned char c)
{
    while (!scr_uart_tx_ready());
        scr_uart_write(DEV_CFG(dev)->regs, SCR_UART_TXD, c);
    return c;
}

#pragma GCC diagnostic pop

static int uart_scr_poll_in(struct device *dev, unsigned char *c)
{

    do {
        *c = scr_uart_getch_nowait();
    } while (*c == -1);

    return 0;
}

static const struct uart_driver_api uart_scr_driver_api = {
	.poll_in = uart_scr_poll_in,
	.poll_out = uart_scr_poll_out,
	.err_check = NULL,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
#error "not implemented yet"
#endif
};

static const struct uart_device_config uart_scr_dev_cfg_0 = {
	.regs = SCR_UART0_PORT,
};

DEVICE_AND_API_INIT(uart_scr_0, CONFIG_UART_CONSOLE_ON_DEV_NAME,
		    uart_scr_init, NULL,
		    &uart_scr_dev_cfg_0,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    (void *)&uart_scr_driver_api);
