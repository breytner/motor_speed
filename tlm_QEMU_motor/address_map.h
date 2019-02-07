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

#define INST_RAM_SIZE 0x0020000
#define INST_RAM_BASEADDR 0x00000000
#define SOFT_SIZE 0x00020000

#define UART_BASEADDR 0x40600000
#define UART_SIZE     0x00010000
#include "hardware/offsets/uart.h"

#endif