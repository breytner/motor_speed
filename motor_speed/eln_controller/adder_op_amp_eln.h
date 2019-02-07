#ifndef ADDER_OP_AMP_ELN_H
#define ADDER_OP_AMP_ELN_H

#include<systemc-ams>

SC_MODULE(adder_op_amp_eln){
    public:
        // Terminals
        sca_eln::sca_terminal va, vb, vc, vo;

        // ELN primitive components
        sca_eln::sca_r ra, rb, rc, ro;
        sca_eln::sca_nullor op_amp;

        adder_op_amp_eln(sc_core::sc_module_name _name, double ra_, double rb_, double rc_, double ro_)
        :   va("va"), vb("vb"), vc("vc"), vo("vo"), ra("ra", ra_), rb("rb", rb_),
            rc("rc", rc_), ro("ro", ro_), op_amp("op_amp"),
            n1("n1"), gnd("gnd")
        {
            ra.p(va);
            ra.n(n1);

            rb.p(vb);
            rb.n(n1);

            rc.p(vc);
            rc.n(n1);

            ro.p(n1);
            ro.n(vo);

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