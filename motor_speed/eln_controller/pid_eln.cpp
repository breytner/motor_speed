#include "pid_eln.h"

pid_eln::pid_eln( sc_core::sc_module_name name, 
    double kp_, double ki_, double kd_)
    : e("e"), u("u"), inv("inv", 1, kp_),
    integrator("integrator", 1, 1.0/ki_),
    differentiator("differentiator", 0.01, 1, kd_),
    adder("adder", 1, 1, 1, 1),
    n1("n1"), n2("n2"), n3("n3"), n4("n4"), gnd("gnd")
{
    inv.vi(e);
    inv.vo(n2);

    integrator.vi(e);
    integrator.vo(n3);

    differentiator.vi(e);
    differentiator.vo(n4);

    adder.va(n2);
    adder.vb(n3);
    adder.vc(n4);
    adder.vo(u);
}