#ifndef INTEGRATOR_OP_AMP_ELN_H
#define INTEGRATOR_OP_AMP_ELN_H

#include<systemc-ams>

SC_MODULE(integrator_op_amp_eln){

    // Terminals
    sca_eln::sca_terminal vi, vo;
    // ELN components
    sca_eln::sca_r r;
    sca_eln::sca_c c;
    sca_eln::sca_nullor op_amp;

    integrator_op_amp_eln(sc_core::sc_module_name _name, double r_, double c_)
    :   vi("vi"), vo("vo"), r("r", r_), c("c", c_), op_amp("op_amp"),
        n1("n1"), gnd("gnd")
    {
        r.p(vi);
        r.n(n1);

        c.p(n1);
        c.n(vo);

        op_amp.nin(n1);
        op_amp.nip(gnd);
        op_amp.nop(vo);
        op_amp.non(gnd);
    }

    private: 
        sca_eln::sca_node n1;
        sca_eln::sca_node_ref gnd;
};

#endif