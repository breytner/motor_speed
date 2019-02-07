#include "ensitlm.h"

#include "rv32im_wrapper.h"
#include "memory.h"
#include "fast-bus.h"
#include "uart.h"
#include "probe.h"
#include "adc.h"
#include "dac.h"
#include "tdf2de.h"
#include "dc_motor.h"
#include "de2tdf.h"

#include<systemc-ams>
#include<chrono>

#include "../address_map.h"

#include "../elf-loader/loader/include/loader.h"
#include "../elf-loader/loader/include/exception.h"


namespace soclib {
namespace common {
extern bool elf_load(const std::string &filename,
                     soclib::common::Loader &loader);
}
};
#define SOFT_SIZE 0xB000

int sc_main(int, char **) {
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	// Signals
	sc_core::sc_signal<double> u_sig;
	sca_tdf::sca_signal<double> u_tdf_sig;
	sca_tdf::sca_signal<double> y_sig;
	sc_core::sc_signal<double> y_de_sig;
	sc_core::sc_signal<bool> adc_irq("adc_irq");

	// Modules
	RV32IMWrapper cpu("riscv");
	Memory inst_ram("inst_ram", INST_RAM_SIZE);
	Probe probe("probe");
	Adc adc("adc");
	Dac dac("dac");
	DcMotor motor("motor");
	De2Tdf de2tdf("de2tdf");
	Tdf2De tdf2de("tdf2de");
	FastBus bus("bus");
	UART uart("uart");

	// Load the program in RAM
	soclib::common::Loader::register_loader("elf",
	                                        soclib::common::elf_load);
	try {
		soclib::common::Loader loader("../software/cross/a.out");
		loader.load(inst_ram.storage, INST_RAM_BASEADDR, SOFT_SIZE);
		for (int i = 0; i < SOFT_SIZE / 4; i++) {
			inst_ram.storage[i] =
			    uint32_le_to_machine(inst_ram.storage[i]);
		}
	} catch (soclib::exception::RunTimeError &e) {
		std::cerr << "unable to load ELF file in memory:" << std::endl;
		std::cerr << e.what() << std::endl;
		abort();
	}

	// initiators
	cpu.socket.bind(bus.target);


	// targets
	bus.initiator(inst_ram.target);
	bus.initiator(probe.target);
	bus.initiator(adc.target);
	bus.initiator(dac.target);
	bus.initiator(uart.target);

	adc.irq(adc_irq);
	cpu.irq(adc_irq);


	//      port             	start addr         	size
	bus.map(inst_ram.target, 	INST_RAM_BASEADDR, 	INST_RAM_SIZE);
	bus.map(probe.target, PROBE_BASEADDR, 	PROBE_SIZE);
	bus.map(adc.target,	 		ADC_BASEADDR,  		ADC_SIZE);
	bus.map(dac.target,	 		DAC_BASEADDR,  		DAC_SIZE);
	bus.map(uart.target, UART_BASEADDR, UART_SIZE);


	///////////////////////////////////////////////////
	// Cyberphysical interconnection			 
	dac.out(u_sig);

	de2tdf.x(u_sig);
	de2tdf.y(u_tdf_sig);

	motor.input_voltage(u_tdf_sig);
	motor.angular_speed(y_sig);
	
	tdf2de.x(y_sig);
	tdf2de.y(y_de_sig);

	adc.in(y_de_sig);
	// End cyberphysical interconnection				 
	///////////////////////////////////////////////////

	// Start tracing a tabular file
	sca_util::sca_trace_file * atf = sca_util::sca_create_tabular_trace_file("trace.dat");
	sca_trace(atf, y_sig ,"motor_speed");
	sca_trace(atf, u_tdf_sig ,"control_signal");

	std::cout << "\n\n*** STARTING SIMULATION\n";

	// start the simulation
	sc_core::sc_start(1.4, sc_core::SC_SEC);

	std::cout << "\n\n*** SIMULATION FINISHED \n";
	std::cout << "*** Simulation results have been written in 'trace.dat'"<< std::endl;
	std::cout << "*** You can run 'octave sim.m' to see them graphically" << std::endl; 

	// Stop tracing
    sca_util::sca_close_tabular_trace_file(atf);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto sim_time = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "*** THE SIMULATION TOOK " << std::dec << sim_time << " MILLISECONDS " << std::endl;

	return 0;
}
