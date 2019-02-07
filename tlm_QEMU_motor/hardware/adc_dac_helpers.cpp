#include"adc_dac_helpers.h"
#include<math.h>
#include<inttypes.h>
#include"../adc_params.h"

float dac_convert(uint32_t val){
    return val * V_REF / pow(2, BITS); 
}

uint32_t adc_convert(float val){
    return val * pow(2, BITS) / V_REF;
}