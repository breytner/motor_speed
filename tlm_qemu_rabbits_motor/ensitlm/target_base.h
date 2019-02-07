#ifndef TARGET_BASE_H
#define TARGET_BASE_H

#include "ensitlm.h"

namespace ensitlm {

class target_base{
    public:
        // Force implementation of read and write methods
        virtual tlm::tlm_response_status read(ensitlm::addr_t a, ensitlm::data_t & d) = 0;
        virtual tlm::tlm_response_status write(ensitlm::addr_t a, ensitlm::data_t d) = 0;

        // Provide and empty implementation to the get_direct_mem_ptr method
        virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &, tlm::tlm_dmi &){
            return false;
        }
};

}
#endif
