#ifndef PID_ELN
#define PID_ELN

#include<systemc-ams>

#include"inverter_op_amp_eln.h"
#include"integrator_op_amp_eln.h"
#include"differentiator_op_amp_eln.h"
#include"adder_op_amp_eln.h"

SC_MODULE(pid_eln){
    public:
        // Terminals
        sca_eln::sca_terminal e, u;

        // ELN components
        inverter_op_amp_eln inv;
        integrator_op_amp_eln integrator;
        differentiator_op_amp_eln differentiator;
        adder_op_amp_eln adder;

        pid_eln(sc_core::sc_module_name name, 
            double kp_ = 1.0, 
            double ki_ = 1.0, 
            double kd_ = 1.0);

    private: 
        sca_eln::sca_node n1, n2, n3, n4;
        sca_eln::sca_node_ref gnd;

};
#endif