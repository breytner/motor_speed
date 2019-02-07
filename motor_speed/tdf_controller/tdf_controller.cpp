#include "tdf_controller.h"

tdf_controller::tdf_controller(sc_core::sc_module_name name, 
    double ref_, double kp_, double ki_, double kd_)
    : y("y"), u("u"), ref_source("ref_source", ref_), subs("subs"),
        tdf_pid("tdf_pid", kp_, ki_, kd_), 
        ref_sig("ref_signal"), err_sig("err_sig")
{
    ref_source.y(ref_sig);

    subs.x1(ref_sig);
    subs.x2(y);
    subs.y(err_sig); 

    tdf_pid.e(err_sig);
    tdf_pid.u(u);
}