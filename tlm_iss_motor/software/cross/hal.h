/*\
 * vim: tw=0: cindent: sw=3: ts=3: sts=3: noet: list
\*/
/********************************************************************
 * Copyright (C) 2009, 2012 by ##LAB_REMOVED_FOR_BLIND_REVIEW##     *
 * Initial author: ##AUTHOR_REMOVED_FOR_BLIND_REVIEW##              *
 ********************************************************************/

/*!
  \file hal.h
  \brief Harwdare Abstraction Layer : implementation for RISC-V
  ISS.


*/
#ifndef HAL_H
#define HAL_H

#include <stdint.h>

/* START CUT */
#define hal_read32(a)      *((volatile uint32_t *)(a))
#define hal_write32(a, d)  do { *((volatile uint32_t *)(a))=(d); } while(0)
#define hal_wait_for_irq() do { irq_received = 0; while (irq_received == 0) {}} while (0)
#define hal_cpu_relax()    do { /* Nothing to do */ } while (0)
/* END CUT */

/* Dummy implementation of abort(): invalid instruction */
#define abort() do {				\
	printf("abort() function called\r\n");  \
	_hw_exception_handler();		\
} while (0)

/* TODO: implement HAL primitives for cross-compilation */
/* UNCOMMENT #define hal_read32(a)      abort() */
/* UNCOMMENT #define hal_write32(a, d)  abort() */
/* UNCOMMENT #define hal_wait_for_irq() abort() */
/* UNCOMMENT #define hal_cpu_relax()    abort() */

static inline void enable_interrupts(void) {
	__asm("li    t0, 0x8\n"
	      "csrs  mstatus, t0\n"
			"li    t0, 0x800\n"
			"csrs  mie, t0");
}

/* TODO: printf is disabled, for now ... */
/* UNCOMMENT #define printf(...) do {} while(0) */
/* START CUT */
#define printf simple_printf
static inline void simple_printf(char *s) {
	/* Assumes that we start with an aligned pointer */
	uint32_t *u = (uint32_t *)s;
	char c;

	do {
		for (int i = 0; i < 4; i++) {
			c = ((*u) >> (i * 8)) & 0xff;
			if (c == 0)
				goto bye;
			hal_write32(UART_BASEADDR + UART_FIFO_WRITE, (uint32_t)(c));
		}
		u++;
	} while (1);
bye:
	c = 0;
}
/* END CUT */

#endif /* HAL_H */
