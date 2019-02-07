#include"dac.h"
#include"ensitlm.h"
#include"offsets/dac.h"
#include"fp_conversions_helpers.h"
#include "../adc_params.h"
#include "adc_dac_helpers.h"


#define TIMESTEP 0.5

// #define DEBUG

Dac::Dac(sc_core::sc_module_name name) : data(0) {
    SC_THREAD(thread_process);
}

void Dac::thread_process(){
    wait(start_event);

    while(true){ 
        uint32_t d = data;
        float out_val = USE_FLOATING_POINT ?  (dac_convert(d) - VOLTAGE_OFFSET) : fpToFloat(d);
        out.write(out_val);
        
#ifdef DEBUG
        std::cout << name() << " writing at output port:  " << out.read() << std::endl;
#endif
        wait(TIMESTEP, sc_core::SC_MS);
    }
}

tlm::tlm_response_status Dac::read(ensitlm::addr_t a, ensitlm::data_t &d) const{
    SC_REPORT_ERROR(name(), "register not implemented");
    return tlm::TLM_ADDRESS_ERROR_RESPONSE;
} 

tlm::tlm_response_status Dac::write(ensitlm::addr_t a, ensitlm::data_t d) {
	switch (a) {
        case DAC_ENA_OFFSET:
            start_event.notify();
            break;
        case DAC_DATA_OFFSET:
            data = d;
            break;
        default:
            SC_REPORT_ERROR(name(), "register not implemented");
            return tlm::TLM_ADDRESS_ERROR_RESPONSE;
	}
	return tlm::TLM_OK_RESPONSE;
}