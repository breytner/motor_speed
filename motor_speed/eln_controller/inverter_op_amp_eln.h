#ifndef INVERTER_OP_AMP_ELN_H
#define INVERTER_OP_AMP_ELN_H

#include<systemc>
#include<systemc-ams>

SC_MODULE(inverter_op_amp_eln){

    // Terminals
    sca_eln::sca_terminal vi, vo;

    // ELN components
    sca_eln::sca_r ra, rb;
    sca_eln::sca_nullor op_amp;

    inverter_op_amp_eln(sc_core::sc_module_name _name, double ra_, double rb_)
    :   vi("vi"), vo("vo"), ra("ra", ra_), rb("rb", rb_), op_amp("op_amp"),
        n1("n1"), gnd("gnd")
    {
        ra.p(vi);
        ra.n(n1);

        rb.p(n1);
        rb.n(vo);

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