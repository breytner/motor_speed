#ifndef ELN_CONTROLLER_H
#define ELN_CONTROLLER_H

#include<systemc>
#include<systemc-ams> 
#include "pid_eln.h"
#include "subtractor_op_amp_eln.h"

SC_MODULE(eln_controller){
    public: 

        // Plant output signal
        sca_tdf::sca_in<double> y;
        // Control signal
        sca_tdf::sca_out<double> u;

        // LSF-TDF interfaces
        sca_eln::sca_tdf::sca_vsource y_conv;
        sca_eln::sca_tdf::sca_vsink u_conv;

        eln_controller(sc_core::sc_module_name, double, double, double, double);       

    private: 
        sca_eln::sca_vsource ref;
        subtractor_op_amp_eln subs; 
        pid_eln eln_pid;

        sca_eln::sca_node y_node, err_node, ref_node, u_node;
        sca_eln::sca_node_ref gnd; 

};
#endif