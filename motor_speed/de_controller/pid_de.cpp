#include "pid_de.h"
#include "../common/constants.h"


pid_de::pid_de(sc_core::sc_module_name name, double kp_, 
    double ki_, double kd_) 
    : e("e"), u("u"), kp(kp_), ki(ki_), kd(kd_),
      last_control(0)
{
    last_errors[0] = 0;
    last_errors[1] = 0;
    SC_METHOD(calc_control_signal);
    sensitive << e;
}

void pid_de::calc_control_signal(){
    double t0 = DE_TIMESTEP*0.001; // Express timestep in seconds

    // Get current error    
    double current_error = e.read();

    double a =  kp + (ki * t0) / 2 + kd / t0;
	double b = -kp + (ki * t0) / 2 - (2 * kd) / t0;
	double c = kd / t0;

	double control = last_control + (a * current_error) + (b * last_errors[0]) + (c * last_errors[1]);

    u.write(control);

	last_errors[1] = last_errors[0];
	last_errors[0] = current_error;
	last_control = control;
    
    /* SECOND VARIANT OF THE SAME ALGORITHM */
    // // Proportional term (avoid derivative action for first iteration)
    // double proportional_action = (kp*t0 + kd)/t0 * current_error;
    
    // // Integral term
    // error_sum += current_error * t0;
    // double integral_action = ki * error_sum;
    
    // // Derivative term
    // double derivative_action = (-kd/t0)*last_error;
    
    // u.write( proportional_action + integral_action + derivative_action );
    // u.write(proportional_action + integral_action + derivative_action);

    // Set up variables for next activation
    // last_error = current_error;
}