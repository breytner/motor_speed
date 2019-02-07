#ifndef DIGITAL_CONTROLLER_H
#define DIGITAL_CONTROLLER_H

#include<systemc>
#include"pid_de.h"

SC_MODULE(digital_controller){
    SC_HAS_PROCESS(digital_controller);
    
    public: 
        // Plant output signal
        sc_core::sc_in<double> y;

        // Control signal
        sc_core::sc_out<double> u;

        // Reference value
        double ref;

        // Processes
        void calc_error_signal();

        digital_controller(sc_core::sc_module_name, double, double, double, double);       

    private: 
        sc_core::sc_signal<double> err_signal;
        pid_de digital_pid;

};
#endif