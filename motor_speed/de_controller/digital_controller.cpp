#include "digital_controller.h"

digital_controller::digital_controller( sc_core::sc_module_name name,
    double ref, double kp_, double ki_, double kd_)
    : ref(ref), digital_pid("digital_pid", kp_, ki_, kd_)
{    
    // Bind pid ports
    digital_pid.e(err_signal);
    digital_pid.u(u);

    // Declare substractor process
    SC_METHOD(calc_error_signal);
    sensitive << y;
}

void digital_controller::calc_error_signal(){
    err_signal.write(ref - y.read() );
}