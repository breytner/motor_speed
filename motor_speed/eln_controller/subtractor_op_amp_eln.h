#ifndef SUBTRACTOR_OP_AMP_ELN_H
#define SUBTRACTOR_OP_AMP_ELN_H

#include<systemc-ams>

SC_MODULE(subtractor_op_amp_eln){

    // Terminals
    sca_eln::sca_terminal va, vb, vo;
    
    // ELN components
    sca_eln::sca_r ra, rb, rc, rd;
    sca_eln::sca_nullor op_amp;

    subtractor_op_amp_eln(sc_core::sc_module_name _name, double ra_ = 1.0, double rb_ = 1.0,
         double rc_ = 1.0, double rd_ = 1.0)
    :   va("va"), vb("vb"), vo("vo"), ra("ra", ra_), rb("rb", rb_),
        rc("rc", rc_), rd("rd", rd_), op_amp("op_amp"),
        n1("n1"), gnd("gnd")
    {
        ra.p(vb);
        ra.n(n1);

        rb.p(n1);
        rb.n(vo);

        rc.p(n2);
        rc.n(va);

        rd.p(n2);
        rd.n(gnd);

        op_amp.nin(n1);
        op_amp.nip(n2);
        op_amp.nop(vo);
        op_amp.non(gnd);
    }

    private: 
        sca_eln::sca_node n1, n2;
        sca_eln::sca_node_ref gnd;
};

#endif