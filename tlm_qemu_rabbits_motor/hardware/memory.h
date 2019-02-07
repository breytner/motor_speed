#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <iomanip>
#include <systemc>
#include <tlm>
#include <array>

using namespace sc_core;
using namespace tlm;

#include "../address_map.h"


#include "ensitlm.h"


class Memory: public sc_module, public ensitlm::target_base
{
    public:
        ensitlm::target_socket<Memory> target;

        std::array<uint8_t, INST_RAM_SIZE> m_array;

        SC_HAS_PROCESS(Memory);
        Memory(const sc_module_name &);
        ~Memory();

        tlm::tlm_response_status read(ensitlm::addr_t a, ensitlm::data_t & d);
        tlm::tlm_response_status write(ensitlm::addr_t a, ensitlm::data_t d);

        bool get_direct_mem_ptr(tlm_generic_payload &, tlm_dmi &);
        
        void load_word(uint32_t, uint32_t);
        void dump();

};

#endif