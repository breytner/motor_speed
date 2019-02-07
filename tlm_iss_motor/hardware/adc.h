#ifndef ADC_H
#define ADC_H

#include<systemc>

#include "ensitlm.h"

struct Adc : sc_core::sc_module {
    public:
        ensitlm::target_socket<Adc> target;
        sc_core::sc_in<double> in;
        sc_core::sc_out<bool> irq;

        tlm::tlm_response_status read(ensitlm::addr_t, ensitlm::data_t &) const;
        tlm::tlm_response_status write(ensitlm::addr_t, ensitlm::data_t);

        void thread_process(void);
        SC_CTOR(Adc);

    private:
        bool intr; // Interruption register
        ensitlm::data_t data;

        sc_core::sc_event start_event;
        void conv_data(void);
}; 

#endif