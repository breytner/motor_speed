#include "memory.h"

Memory::Memory(const sc_module_name &name)
        : sc_module(name)
    {
        // p_socket.bind(*this);
        std::memset(&m_array[0], 0, m_array.size());
    }


Memory::~Memory(){ }

// Read transactions
tlm::tlm_response_status Memory::read(ensitlm::addr_t a, ensitlm::data_t &d) {
    std::cout << "MEMORY READ" << std::endl;
    abort();
}

// Write transactions
tlm::tlm_response_status Memory::write(ensitlm::addr_t a, ensitlm::data_t d) {
    std::cout << "MEMORY WRITE" << std::endl;
    abort();
}

bool Memory::get_direct_mem_ptr(tlm_generic_payload &payload, tlm_dmi &dmi_data)
{
    std::cout << "DMI access " << payload.get_address() << "\n";
    if (payload.get_address() >= m_array.size()) {
        return false;
    }
    dmi_data.set_start_address(INST_RAM_BASEADDR);
    dmi_data.set_end_address(INST_RAM_BASEADDR + m_array.size() - 1);
    dmi_data.set_dmi_ptr(&m_array[0]);
    dmi_data.allow_read();
    dmi_data.allow_write();
    return true;
}

void Memory::load_word(uint32_t position, uint32_t word){
    if (position + 3 >= m_array.size()) {
        std::cerr << "unable to load word file in memory:" << std::endl;
        std::cerr << "invalid position" << std::endl;
        abort();
    }
    m_array[position] = word & 0xFF;
    m_array[position+1] = (word & 0xFF00) >> 8;
    m_array[position+2] = (word & 0xFF0000) >> 16;
    m_array[position+3] = (word & 0xFF000000) >> 24;
}

void Memory::dump(){
    std::cout << "*** INSTRUCTION MEMORY CONTENTS: " << std::endl;
    int position = 0;
    for (const auto& s: m_array) {
        std::cout << std::dec << std::setw(5) << std::left << position++ << (sc_dt::sc_bv<8>) s << std::endl;
    }
}