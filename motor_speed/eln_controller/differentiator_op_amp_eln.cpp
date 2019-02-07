#include"differentiator_op_amp_eln.h"

differentiator_op_amp_eln::differentiator_op_amp_eln(
    sc_core::sc_module_name _name, 
    double ra_, double rb_, double c_)
    : vi("vi"), vo("vo"), ra("ra", ra_), 
    rb("rb", rb_), c("c", c_), op_amp("op_amp"),
    n1("n1"), n2("n2"), gnd("gnd")
{
    ra.p(vi);
    ra.n(n1);

    c.p(n1);
    c.n(n2);

    rb.p(n2);
    rb.n(vo);
    
    op_amp.nin(n2);
    op_amp.nip(gnd);
    op_amp.nop(vo);
    op_amp.non(gnd);
}