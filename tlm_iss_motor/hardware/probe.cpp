#include "ensitlm.h"
#include "probe.h"
#include "../address_map.h"
#include "fp_conversions_helpers.h"

// Constructor
Probe::Probe(sc_core::sc_module_name name) {
}

// Destructor
Probe::~Probe() {
}

// Read transactions
tlm::tlm_response_status Probe::read(ensitlm::addr_t a, ensitlm::data_t &d) {
    SC_REPORT_ERROR(name(), "read not implemented");
    return tlm::TLM_ADDRESS_ERROR_RESPONSE;
}

// Write transactions
tlm::tlm_response_status Probe::write(ensitlm::addr_t a, ensitlm::data_t d) {
    std::cout << name() << " writing " << std::dec << (int) d << " | \t"<<  fpToFloat(d) << std::endl;
    return tlm::TLM_OK_RESPONSE;
}