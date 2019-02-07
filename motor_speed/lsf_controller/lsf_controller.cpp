#include "lsf_controller.h"

lsf_controller::lsf_controller(sc_core::sc_module_name name,
    double ref_, double kp_, double ki_, double kd_)
    : y("y"), u("u"), y_conv("y_conv"), u_conv("u_conv"),
        ref("ref", 0.0, ref_, 0.0, 0.0, 0.0), subs("subs"), 
        lsf_pid("lsf_pid", kp_, ki_, kd_), 
        y_lsf_sig("y_lsf_sig"), u_lsf_sig("u_lsf_sig"),
        ref_sig("ref_sig"), err_sig("err_sig")
{
    y_conv.inp(y);
    y_conv.y(y_lsf_sig);

    ref.y(ref_sig);

    subs.x1(ref_sig);
    subs.x2(y_lsf_sig);
    subs.y(err_sig);

    lsf_pid.e(err_sig);
    lsf_pid.u(u_lsf_sig);

    u_conv.x(u_lsf_sig);
    u_conv.outp(u);
}