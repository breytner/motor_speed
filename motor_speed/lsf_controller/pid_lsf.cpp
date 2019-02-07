#include "pid_lsf.h"

#define DEFAULT_

pid_lsf::pid_lsf( sc_core::sc_module_name, double kp_, double ki_, double kd_)
    : e("e"), u("u"),
    add_1("add_1"), add_2("add_2"), kp_gain("kp", kp_), 
    integ_1("integ_1", ki_), dot_1("dot_1", kd_),
    sig_p("sig_p"), sig_i("sig_i"), sig_d("sig_d"),
    sig_pi("sig_pi")
{        
    kp_gain.x(e);
    kp_gain.y(sig_p);

    integ_1.x(e);
    integ_1.y(sig_i);

    dot_1.x(e);
    dot_1.y(sig_d);

    add_1.x1(sig_p);
    add_1.x2(sig_i);
    add_1.y(sig_pi);

    add_2.x1(sig_pi);
    add_2.x2(sig_d);
    add_2.y(u);
}