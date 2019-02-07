// hal.h depends on constants declared on address_map.h
// so it has to be included second

#include "address_map.h"

#include "hal.h"

#include "fixed_point_params.h"
#include "adc_params.h"


volatile int irq_received = 0;

// Controller parameters converted to signed fixed point Q21.10
#define KP_FP  3072 // 1536 // 768 // 3.0 
#define KI_FP 15360 // 3840 // 15.0 
#define KD_FP 307 // 76 // 0.3  
#define T0_FP  10 // 2 // 0.0078125 s 
#define REF_FP 1024 // 256 // 4096
#define FP_2 2048 //512
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


int main(){

	enable_interrupts();

	short converters_enabled = 0;
	
	while(1){
		if (!converters_enabled){
			// Enable ADC
			hal_write32(ADC_BASEADDR + ADC_ENA_OFFSET, 1);
			/// Enable DAC
			hal_write32(DAC_BASEADDR + DAC_ENA_OFFSET, 1);
			converters_enabled = 1;
		}

		hal_wait_for_irq();
	}

    return 0;
}

void interrupt_handler(){

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

// ADC interuption service routine using floating point unit
void adc_isr_floating_point(){
	return;
}


// ADC interuption service routine using fixed point arithmetics
void adc_isr_fixed_point(){
	int32_t y = hal_read32(ADC_BASEADDR + ADC_DATA_OFFSET) - FP_VOLTAGE_OFFSET;

	static int32_t last_error_0 = 0, last_error_1 = 0, last_control = 0;

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
		+ fpMlt(b, last_error_0) + fpMlt(c, last_error_1);

	// Write back control signal
	hal_write32(DAC_BASEADDR + DAC_DATA_OFFSET, u);

	// Set up variables for next activation
	last_error_1 = last_error_0;
	last_error_0 = current_error;
	last_control = u;


	return;
}

// Avoid overflows in fixed point (FP) operations 
fixed_point_t avoid_overflow( fixed_point_2x_t value ){
	// saturate the result before assignment
	if( value > INT32_MAX){
		// simple_printf("***ERROR:");
		return INT32_MAX;
    }
    if( value < INT32_MIN){
		// printf("\n***ERROR: saturated MIN value\n");
		return INT32_MIN;
    }
    return value;
}

// Multiplication
fixed_point_t fpMlt(fixed_point_t a, fixed_point_t b){
    fixed_point_2x_t temp = (fixed_point_2x_t) a * b;

	// add 1/2 to give correct rounding
    temp = temp + (1 << (FIXED_POINT_N-1) ); 

    // divide by 2^N
    temp = temp >> FIXED_POINT_N;   
	
	return avoid_overflow(temp);
}

// Division
fixed_point_t fpDiv(fixed_point_t a, fixed_point_t b){
    // perform the division
    fixed_point_2x_t temp = (a << FIXED_POINT_N) / b;
	
	return avoid_overflow(temp);
}