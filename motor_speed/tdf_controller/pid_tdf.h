#ifndef PID_TDF_H
#define PID_TDF_H

#include<systemc-ams>

SCA_TDF_MODULE(pid_tdf){

    // Ports
    sca_tdf::sca_in<double> e;
    sca_tdf::sca_out<double> u;

    // Parameters
    double kp, ki, kd;

    pid_tdf( sc_core::sc_module_name name_, 
        double kp_ = 1.0, double ki_ = 0.0, double kd_ = 0.0 );

    void initialize();
    void processing();

    private: 
        // Transfer function definition
        // (Numerator and denominator coefficients)
        sca_util::sca_vector<double> num, den;

        // Laplace transfer funtion
        sca_tdf::sca_ltf_nd ltf_nd; 

};

#endif