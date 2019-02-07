#include "adc.h"
#include "ensitlm.h"
#include "offsets/adc.h"
#include "fp_conversions_helpers.h"
#include "../fixed_point_params.h"
#include "../adc_params.h"
#include "adc_dac_helpers.h"

// #define DEBUG


Adc::Adc(sc_core::sc_module_name name) : intr(0){
    SC_THREAD(thread_process);
}

void Adc::thread_process(){
    wait(start_event);
    // Timestep in miliseconds 
    uint32_t timestep = USE_FLOATING_POINT ? 10 : 9.765626;

    while(true){ 
        if( !intr ){
            intr = true;  
            conv_data();
            irq.write(1);
            wait(20, sc_core::SC_NS);
            irq.write(0); 
        }
        wait(timestep, sc_core::SC_MS);
    }
}


void Adc::conv_data(){
    float x = in.read() + VOLTAGE_OFFSET;
    
    data = USE_FLOATING_POINT ? (uint32_t) adc_convert(x) : floatToFp(x); 
#ifdef DEBUG
    std::cout << name() << " converting " << in.read() << " to " << std::dec << (int32_t) data << std::endl;
#endif
}

tlm::tlm_response_status Adc::read(ensitlm::addr_t a, ensitlm::data_t &d) const{
    switch (a) {
        case ADC_DATA_OFFSET:
            d = data;
            break;
        case ADC_INT_OFFSET:
            d = intr;
            break;
        default:
            SC_REPORT_ERROR(name(), "register not implemented");
            return tlm::TLM_ADDRESS_ERROR_RESPONSE;
    }
    return tlm::TLM_OK_RESPONSE;  
}

tlm::tlm_response_status Adc::write(ensitlm::addr_t a, ensitlm::data_t d) {
	switch (a) {
        case ADC_ENA_OFFSET:
            start_event.notify();
            break;
        case ADC_INT_OFFSET:
            intr = false;
            break;
        default:
            SC_REPORT_ERROR(name(), "register not implemented");
            return tlm::TLM_ADDRESS_ERROR_RESPONSE;
	}
	return tlm::TLM_OK_RESPONSE;
}