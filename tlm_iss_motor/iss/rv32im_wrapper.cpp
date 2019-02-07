/*\
 * vim: tw=0: cindent: sw=3: ts=3: sts=3: noet: list
 * Wrapper of Soclib ISS for the ensitlm protocol
 * Largely inspired from the Microblaze Wrapper by ##AUTHOR_REMOVED_FOR_BLINF_REVIEW## 
 * Contributing authors: ##AUTHOR_REMOVED_FOR_BLIND_REVIEW##
 *
\*/

#include "ensitlm.h"
#include "rv32im_wrapper.h"
#include "rv32im.h"
#include <iomanip>

#if 0
#define DEBUG
#endif
#define NB_INST 50
static const sc_core::sc_time PERIOD_MULT(20 * NB_INST, sc_core::SC_NS);

/* Time between two step()s */
static const sc_core::sc_time PERIOD(20, sc_core::SC_NS);

using namespace std;

RV32IMWrapper::RV32IMWrapper(sc_core::sc_module_name name)
	: sc_core::sc_module(name), irq("irq"),
	m_iss(0) /* identifier, not very useful since we have only one instance */
{
	m_iss.reset();
	m_iss.setIrq(false);
	SC_THREAD(run_iss);
	/* The method that is needed to forward the interrupts from the SystemC
	 * environment to the ISS need to be declared here */
	SC_METHOD(process_irq);
	dont_initialize();
	sensitive << irq;
}

/* IRQ forwarding method to be defined here */
void RV32IMWrapper::process_irq(void)
{
#ifdef INFO
	std::cout << name() << ": IRQ signal changed to " << irq.read()
	          << std::endl;
#endif
	if (irq.read()) {
		m_iss.setIrq(true);
		irq_duration = 10;
	}
}

void RV32IMWrapper::exec_data_request(enum iss_t::DataAccessType mem_type,
                                  uint32_t mem_addr, uint32_t mem_wdata)
{
	uint32_t localbuf;
	int      shift;
	tlm::tlm_response_status status;

	switch (mem_type) {
		case iss_t::READ_WORD:
			status = socket.read(mem_addr, localbuf);
			assert(status == tlm::TLM_OK_RESPONSE);
			localbuf = uint32_machine_to_le(localbuf);
#ifdef DEBUG
			std::cout << hex << "read    " << setw(10) << localbuf
						 << " at address " << mem_addr << std::endl;
#endif
			m_iss.setDataResponse(0, localbuf);
			break;
		case iss_t::READ_BYTE:
			status = socket.read(mem_addr & (~0x3), localbuf);
			assert(status == tlm::TLM_OK_RESPONSE);
			localbuf = uint32_machine_to_le(localbuf);
			shift = (mem_addr & 0x3) * 8;
			localbuf = (localbuf >> shift) & 0xFF;
#ifdef DEBUG
			std::cout << hex << "read    " << setw(10) << localbuf
						 << " at address " << mem_addr << std::endl;
#endif
			m_iss.setDataResponse(0, localbuf);
			break;
		case iss_t::WRITE_HALF:
		case iss_t::WRITE_BYTE:
		case iss_t::READ_HALF:
			// Not needed for our platform.
			std::cerr << "Operation " << mem_type << " unsupported for "
						 << std::showbase << std::hex << mem_addr << std::endl;
			abort();
		case iss_t::LINE_INVAL:
			// No cache => nothing to do.
			break;
		case iss_t::WRITE_WORD:
			/* The ISS requested a data write
				(mem_wdata at mem_addr). */
			status = socket.write(mem_addr, uint32_le_to_machine(mem_wdata));
			assert(status == tlm::TLM_OK_RESPONSE);
#ifdef DEBUG
			std::cout << hex << "wrote   " << setw(10) << mem_wdata
						 << " at address " << mem_addr << std::endl;
#endif
			m_iss.setDataResponse(0, 0);
			break;
		case iss_t::STORE_COND:
			break;
		case iss_t::READ_LINKED:
			break;
	}
}

void RV32IMWrapper::run_iss(void)
{
	int inst_count = 0;
	int total_insns = 0;

	while (true) {
		if (m_iss.isBusy())
			m_iss.nullStep();
		else {
			bool ins_asked;
			uint32_t ins_addr;
			m_iss.getInstructionRequest(ins_asked, ins_addr);

			if (ins_asked) {
				/* The ISS requested an instruction.
				 * We have to do the instruction fetch
				 * by reading from memory. */
				/* UNCOMMENT abort(); // TODO */
				uint32_t localbuf;
				tlm::tlm_response_status status = socket.read(ins_addr, localbuf);
				assert(status == tlm::TLM_OK_RESPONSE);
				localbuf = uint32_machine_to_le(localbuf);
				m_iss.setInstruction(0, localbuf);
			}

			bool mem_asked;
			enum iss_t::DataAccessType mem_type;
			uint32_t mem_addr;
			uint32_t mem_wdata;
			m_iss.getDataRequest(mem_asked, mem_type, mem_addr, mem_wdata);

			if (mem_asked) {
				exec_data_request(mem_type, mem_addr, mem_wdata);
			}
			m_iss.step();

			/* IRQ handling to be done */
			if (irq_duration > 0) {
				irq_duration--;
			} else if (irq_duration == 0) {
				m_iss.setIrq(false);
				irq_duration = -1;
			}
		}

		wait(PERIOD);
		if (++inst_count >= NB_INST) {
			inst_count = 0;
			wait(PERIOD_MULT);
			total_insns += NB_INST;
#if 0
			if (total_insns > 10000 * NB_INST)
				exit(1);
#endif
		}
	}
}
