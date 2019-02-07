/*
 * vim: tw=0: cindent: sw=3: ts=3: sts=3: noet: list
 */
#include "../address_map.h"
#include "../fixed_point_params.h"
#include "../adc_params.h"

#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

volatile int irq_received = 0;

#include "hal.h"

// Controller parameters
#define KP 3.0f
#define KI 15.0f
#define KD 0.3f
#define T0 0.01f

// Controller parameters converted to signed fixed point Q21.10
#define KP_FP 3072 // 3.0 
#define KI_FP 15360 // 15.0 
#define KD_FP 307 // 0.3  
#define T0_FP 10 // 0.0078125 s 
#define REF_FP 1024 //
#define FP_2 2048 // 
#define FP_VOLTAGE_OFFSET 5120

///////////////////////////////////////////////////////////////
// Function prototypes
fixed_point_t avoid_overflow(fixed_point_2x_t);
fixed_point_t fpMlt(fixed_point_t, fixed_point_t);
fixed_point_t fpDiv(fixed_point_t, fixed_point_t);
void adc_isr_fixed_point();
void adc_isr_floating_point();
void interrupt_handler();
// End function prototypes
////////////////////////////////////////////////////////////////

int main(void){
	printf("-- boot complete --\r\n");
    
	enable_interrupts();


	int converters_enabled = 0;

	printf("-- entering main loop --\r\n");
	while (1) {
		while (1) {
			if (!converters_enabled){
				// Enable ADC
				hal_write32(ADC_BASEADDR + ADC_ENA_OFFSET, 1);
				/// Enable DAC
				hal_write32(DAC_BASEADDR + DAC_ENA_OFFSET, 1);
			}
			
			hal_wait_for_irq(); 
		}
	}

	return 0;
}

void interrupt_handler() {
	irq_received = 1;

	// check if ADC requested interrupt
	uint32_t adc_irq = hal_read32(ADC_BASEADDR + ADC_INT_OFFSET);
	if (adc_irq) {
		// call the service routine
		if (USE_FLOATING_POINT) {
			adc_isr_floating_point();
		}
		else{
			adc_isr_fixed_point();
		}
		// clear interrupt
		hal_write32(ADC_BASEADDR + ADC_INT_OFFSET, adc_irq);
	}
}

// ADC interuption service routine using fixed point arithmetics
void adc_isr_fixed_point(){
	static int32_t last_errors[2] = {0, 0}, last_control = 0;

	int32_t y = hal_read32(ADC_BASEADDR + ADC_DATA_OFFSET) - FP_VOLTAGE_OFFSET;

    // Calculate current error    
    int32_t current_error = REF_FP - y;

	// Calculate coefficients for the control signal equation
	int32_t temp1 = fpDiv(fpMlt(KI_FP, T0_FP), FP_2); //((fpMlt(KI_FP, T0_FP) << FIXED_POINT_N) + (FP_2 >> 1)) / FP_2; // (KI*T0/2)
	int32_t temp2 = fpDiv(KD_FP, T0_FP); // ((KD_FP << FIXED_POINT_N) + (T0_FP >> 1)) / T0_FP; // KD/T0
	int32_t temp3 = fpMlt(temp2, FP_2); // 2*KD/T0
	int32_t a = KP_FP + temp1 + temp2; // KP + KI*T0/2 + KD/T0
	int32_t b = -KP_FP + temp1 - temp3; // -KP + KI*T0/2 + -2*KD/T0
	int32_t c = temp2; // KD/T0
	int32_t u = last_control + fpMlt(a, current_error) 
		+ fpMlt(b, last_errors[0]) + fpMlt(c, last_errors[1]);

	// Write back control signal
	hal_write32(DAC_BASEADDR + DAC_DATA_OFFSET, u);

	// Set up variables for next activation
	last_errors[1] = last_errors[0];
	last_errors[0] = current_error;
	last_control = u;

	// Debugging with probe
	// hal_write32(PROBE_BASEADDR, y); 
}

// ADC interuption service routine using floating point unit
void adc_isr_floating_point(){
	// ////////////////////////////////////
	// // Errors
	// float a = 5.0f;
	// float b = 6.0f;
	// uint32_t r = a < b;
	// uint32_t check = r == (5.0f < 6.0f);
	// // Check must be 1, but it is 0
	// hal_write32(PROBE_BASEADDR, r);
	//
	// Additional info: the following code correctly works
	// float a = 5.0f;
	// float b = 6.0f;
	// float c = a < b;
	// uint32_t r = c;
	// uint32_t check = r == (5.0f < 6.0f);
	// // Check must be 1, but it is 0
	// hal_write32(PROBE_BASEADDR, r);
	// 
	// // end Errors
	// /////////////////////////////////////
	
	static float last_errors[2] = {0.0f, 0.0f}, last_control = 0.0f;
	 
	float y = (float) hal_read32(ADC_BASEADDR + ADC_DATA_OFFSET) * V_REF / NUMBER_OF_LEVELS;
	y = y - VOLTAGE_OFFSET;
	float current_error = 1.0f - y;

	float a =  KP + (KI * T0) / 2 + KD / T0;
	float b = -KP + (KI * T0) / 2 - (2 * KD) / T0;
	float c = KD / T0;

	float u = last_control + (a * current_error) + (b * last_errors[0]) + (c * last_errors[1]);

	// There seems to be an error when invoking adc_convert
	// For example, adc_convert(u) freezes the execution
	// The following code is equivalent to adc_convert
	uint32_t u_uint = (u + VOLTAGE_OFFSET) * (NUMBER_OF_LEVELS / V_REF); 
	hal_write32(DAC_BASEADDR + DAC_DATA_OFFSET, u_uint);

	last_errors[1] = last_errors[0];
	last_errors[0] = current_error;
	last_control = u;

	// Debugging
	// hal_write32(PROBE_BASEADDR, y);
}


// Avoid overflows in fixed point (FP) operations 
fixed_point_t avoid_saturation( fixed_point_2x_t value ){
	// saturate the result before assignment
    if( value > INT32_MAX){
		printf("\n***ERROR: saturated MAX value\n");
	    return INT32_MAX;
    }
    if( value < INT32_MIN){
		printf("\n***ERROR: saturated MIN value\n");
        return INT32_MIN;
    }
    return value;
}

// Addition
fixed_point_t fpAdd(fixed_point_t a, fixed_point_t b){
    return a + b;
}

// Subtraction
fixed_point_t fpSub(fixed_point_t a, fixed_point_t b){
    return a - b;
}

// Multiplication
fixed_point_t fpMlt(fixed_point_t a, fixed_point_t b){
    fixed_point_2x_t temp = (fixed_point_2x_t) a * b;

	// add 1/2 to give correct rounding
    temp = temp + (1 << (FIXED_POINT_N-1) ); 

    // divide by 2^N
    temp = temp >> FIXED_POINT_N;   
	
	return avoid_saturation(temp);
}

// Division
fixed_point_t fpDiv(fixed_point_t a, fixed_point_t b){
    // perform the division
    fixed_point_2x_t temp = (a << FIXED_POINT_N) / b;
	
	// add b/2 to give correct rounding
    // temp += b >> 1;
	
	return avoid_saturation(temp);
}