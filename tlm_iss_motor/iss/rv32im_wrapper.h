#ifndef RV32IM_WRAPPER_H
#define RV32IM_WRAPPER_H

#include "ensitlm.h"

#include "rv32im.h"

/*!
  Wrapper for Rv32im ISS using the BASIC protocol.
*/
struct RV32IMWrapper : sc_core::sc_module {
	ensitlm::initiator_socket<RV32IMWrapper> socket;
	sc_core::sc_in<bool> irq;

	void run_iss(void);
	void process_irq(void);
	int irq_duration;

	SC_CTOR(RV32IMWrapper);

private:
	typedef soclib::common::Rv32imIss iss_t;
	void exec_data_request(enum iss_t::DataAccessType mem_type,
	                       uint32_t mem_addr, uint32_t mem_wdata);

	iss_t m_iss;
};

#endif // RV32IM_WRAPPER_H
