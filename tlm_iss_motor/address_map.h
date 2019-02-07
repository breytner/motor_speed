#ifndef ADDRESS_MAP_H
#define ADDRESS_MAP_H

#define PROBE_BASEADDR 0xF0000000
#define PROBE_SIZE 0x0000000F

#define ADC_BASEADDR 0xA0000000
#define ADC_SIZE 0x00010000 
#include "hardware/offsets/adc.h"

#define DAC_BASEADDR 0xB0000000
#define DAC_SIZE 0x00010000 
#include "hardware/offsets/dac.h"   

#define INST_RAM_SIZE (128 * 1024)
#define INST_RAM_BASEADDR 0x80000000

// #define TIMER_BASEADDR 0x41C00000
// #define TIMER_SIZE     0x00010000
// #include "hardware/offsets/timer.h"

#define UART_BASEADDR 0x40600000
#define UART_SIZE     0x00010000
#include "hardware/offsets/uart.h"

#endif