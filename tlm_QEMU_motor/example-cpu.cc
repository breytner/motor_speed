#include <cstdio>
#include <iostream>
#include <systemc>
#include <tlm>
#include <cpu-wrapper.h>
#include <chrono>

#include "./elf-loader/loader/include/loader.h"
#include "./elf-loader/loader/include/exception.h"

#include "memory.h"
#include "fast_bus.h"
#include "uart.h"
#include "probe.h"
#include "adc.h"
#include "dac.h"
#include "tdf2de.h"
#include "dc_motor.h"
#include "de2tdf.h"

// FUNCTION PROTOTYPES
void load_instructions(Memory *);

using namespace sc_core;

// MAIN
int sc_main(int argc, char *argv[])
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	// SIGNALS
	sc_core::sc_signal<double> u_sig;
	sca_tdf::sca_signal<double> u_tdf_sig;
	sca_tdf::sca_signal<double> y_sig;
	sc_core::sc_signal<double> y_de_sig;
	sc_core::sc_signal<bool> adc_irq("adc_irq");


    // COMPONENT INSTANTIATION
    Memory inst_ram("inst_ram");
    RiscVCpuWrapper *cpu = new RiscVCpuWrapper("cpu");
    FastBus bus("bus");
	UART uart("uart");
	Probe probe("probe");
	Adc adc("adc");
	Dac dac("dac");
	DcMotor motor("motor");
	De2Tdf de2tdf("de2tdf");
	Tdf2De tdf2de("tdf2de");

    // INSTRUCTIONS LOADING    
    load_instructions(&inst_ram);

    // BINDINGS
	// initiators
	cpu->p_socket.bind(bus.target);

	// targets
	bus.initiator(inst_ram.target);
	bus.initiator(uart.target);
	bus.initiator(adc.target);
	bus.initiator(dac.target);
	bus.initiator(probe.target);

	// Memory mapping
	//      port             	start addr         		size
	bus.map(inst_ram.target,	INST_RAM_BASEADDR, 		INST_RAM_SIZE);
	bus.map(uart.target, 		UART_BASEADDR, 			UART_SIZE);
	bus.map(probe.target, 		PROBE_BASEADDR, 		PROBE_SIZE);
	bus.map(adc.target,	 		ADC_BASEADDR,  			ADC_SIZE);
	bus.map(dac.target,	 		DAC_BASEADDR,  			DAC_SIZE);
	
	// IRQ CONNECTION
	cpu->p_irq.bind(adc_irq); 
	adc.irq.bind(adc_irq);

	// INTERCONNECTION AMONG SYSTEMC AMS COMPONENTS			 
	dac.out(u_sig);

	de2tdf.x(u_sig);
	de2tdf.y(u_tdf_sig);

	motor.input_voltage(u_tdf_sig);
	motor.angular_speed(y_sig);
	
	tdf2de.x(y_sig);
	tdf2de.y(y_de_sig);

	adc.in(y_de_sig);

    // SIMULATION
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
    std::cout << "*** THE SIMULATION TOOK " << std::dec << sim_time << " MILLISECONDS" << std::endl;

    return 0;
}


// FUNCTION DEFINITIONS
void load_instructions(Memory *mem){
    // Load the program in RAM
	soclib::common::Loader::register_loader("elf",
	                                        soclib::common::elf_load);
    try {
		soclib::common::Loader loader("./cross/a.out");
        uint32_t temp_storage[INST_RAM_SIZE / sizeof(uint32_t)];
		loader.load(temp_storage, INST_RAM_BASEADDR, SOFT_SIZE);
		for (int i = 0; i < SOFT_SIZE / 4; i++) {
            mem->load_word(i*4, temp_storage[i]);
		}
	} catch (soclib::exception::RunTimeError &e) {
		std::cerr << "unable to load ELF file in memory:" << std::endl;
		std::cerr << e.what() << std::endl;
		abort();
	}
}