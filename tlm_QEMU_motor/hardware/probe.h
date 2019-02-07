/*************************************************************
 * Definition of a hypothetic probe module.  It serves as a 
 * measurement instrument for variables in the main.c code 
 * for debugging purposes.
 * It is not part of the design. 
 * ***********************************************************/

#ifndef PROBE_H
#define PROBE_H

#include "ensitlm.h"

struct Probe : public sc_core::sc_module, public ensitlm::target_base {
    public:
        ensitlm::target_socket<Probe> target;

        Probe(sc_core::sc_module_name name);

        ~Probe();

        tlm::tlm_response_status read(ensitlm::addr_t a, ensitlm::data_t & d);

        tlm::tlm_response_status write(ensitlm::addr_t a, ensitlm::data_t d);
};

#endif
