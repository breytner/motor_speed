#include<systemc>
#include<systemc-ams>
#include<iostream>

#include "../motor/dc_motor_tdf.h"
#include "de2tdf_converter.h"
#include "tdf2de_converter.h"
#include "digital_controller.h"
#include "../common/constants.h"


int sc_main(int argc, char* argv[]){

    sc_core::sc_set_time_resolution(10.0, sc_core::SC_NS);

    sca_tdf::sca_signal<double> sig_ref, motor_speed_0;
    sca_tdf::sca_signal<double> e_de_unconverted, u_de_converted;
    sc_core::sc_signal<double> motor_speed_de, u_de_unconverted;

    // DE controller
    tdf2de_converter tdf2de("tdf2de");
    tdf2de.x(motor_speed_0);
    tdf2de.y(motor_speed_de);

    digital_controller controller_de("controller_de", REF_VAL, KP, KI, KD);
    controller_de.y(motor_speed_de);
    controller_de.u(u_de_unconverted);

    de2tdf_converter de2tdf("de2tdf");
    de2tdf.x(u_de_unconverted);
    de2tdf.y(u_de_converted); 

    dc_motor_tdf dc_motor_0("dc_motor_0");
    dc_motor_0.input_voltage(u_de_converted);
    dc_motor_0.angular_speed(motor_speed_0);

    // Start tracing a tabular file
    sca_util::sca_trace_file * atf = sca_util::sca_create_tabular_trace_file("trace.dat");
    sca_trace(atf, motor_speed_0 ,"motor_speed_0");
    
    // Start simulation for a given time
    sc_core::sc_start(SIMULATION_TIME_MS, sc_core::SC_MS);

    // Stop tracing
    sca_util::sca_close_tabular_trace_file(atf);

    std::cout << "\n\n*** SIMULATION FINISHED \n";
	std::cout << "*** Simulation results have been written in 'trace.dat'"<< std::endl;
	std::cout << "*** You can run 'octave sim.m' to see them graphically" << std::endl; 

    return 0;
}