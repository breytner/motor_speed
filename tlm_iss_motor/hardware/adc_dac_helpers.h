#ifndef ADC_DAC_HELPERS_H
#define ADC_DAC_HELPERS_H

#include<inttypes.h>

float dac_convert(uint32_t);
uint32_t adc_convert(float);

#endif