#ifndef DIFFERENTIATOR_OP_AMP_ELN_H
#define DIFFERENTIATOR_OP_AMP_ELN_H

#include<systemc-ams>

SC_MODULE(differentiator_op_amp_eln){
    public:
        // Terminals
        sca_eln::sca_terminal vi, vo;

        // ELN primitive components
        sca_eln::sca_r ra, rb;
        sca_eln::sca_c c;
        sca_eln::sca_nullor op_amp;

        differentiator_op_amp_eln(sc_core::sc_module_name _name, 
            double ra_, double rb_, double c_);

    private: 
        sca_eln::sca_node n1, n2;
        sca_eln::sca_node_ref gnd;
};

#endif