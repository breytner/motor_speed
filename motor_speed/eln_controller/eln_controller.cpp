#include "eln_controller.h"

eln_controller::eln_controller(sc_core::sc_module_name name,
    double ref_, double kp_, double ki_, double kd_)
    : y("y"), u("u"), y_conv("y_conv"), u_conv("u_conv"),
        ref("ref", 0, ref_), subs("subs"),
        eln_pid("eln_pid", kp_, ki_, kd_ ),
        y_node("y_node"), err_node("err_node"), ref_node("ref_node"),
        u_node("u_node"), gnd("gnd")
{
    y_conv.inp(y);
    y_conv.p(y_node);
    y_conv.n(gnd);

    ref.p(ref_node);
    ref.n(gnd);

    subs.va(ref_node);
    subs.vb(y_node);
    subs.vo(err_node);

    eln_pid.e(err_node); 
    eln_pid.u(u_node); 

    u_conv.p(u_node); 
    u_conv.n(gnd);
    u_conv.outp(u);
}