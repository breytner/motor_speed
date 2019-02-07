#ifndef DAC_H
#define DAC_H

#include<systemc>

#include "ensitlm.h"

struct Dac : sc_core::sc_module {
    public:
        ensitlm::target_socket<Dac> target;

        sc_core::sc_out<double> out;

        tlm::tlm_response_status read(ensitlm::addr_t, ensitlm::data_t &) const;
        tlm::tlm_response_status write(ensitlm::addr_t, ensitlm::data_t);

        void thread_process(void);
        SC_CTOR(Dac);

    private:
        ensitlm::data_t data;

        sc_core::sc_event start_event;
        void conv_data(void);
}; 

#endif