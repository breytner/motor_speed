#ifndef PID_H
#define PID_H

#include<systemc-ams>

#include<iostream>

// TODO: put in a different file
// TODO: remove hardcoded parameters
SC_MODULE(practical_differentiator_lsf){
    // Ports
    sca_lsf::sca_in x;
    sca_lsf::sca_out y;

    double k;

    practical_differentiator_lsf( sc_core::sc_module_name, double k_ = 1.0)
    : x("x"), y("y"), k(k_), ltf_nd("ltf_nd")
    {
        sca_util::sca_vector<double> num, den;        
        
        double r = 0.01 , rf = 1, c = 0.3;
        num(0) = 0; 
        num(1) = rf*c;
        den(0) = 1;
        den(1) = r*c;
        
        ltf_nd.num = num;
        ltf_nd.den = den; 
        ltf_nd.k = 1;

        ltf_nd.x(x); 
        ltf_nd.y(y);
    } 

    private:
    sca_lsf::sca_ltf_nd ltf_nd;
}; 


SC_MODULE(pid_lsf){
    public:
        // Ports
        sca_lsf::sca_in e;
        sca_lsf::sca_out u;

        // Blocks
        sca_lsf::sca_add add_1, add_2;
        sca_lsf::sca_gain kp_gain;
        sca_lsf::sca_integ integ_1;
        practical_differentiator_lsf dot_1;

        pid_lsf( sc_core::sc_module_name, 
            double kp_ = 1.0, double ki_ = 0, double kd_ = 0);
    
    private: 
        sca_lsf::sca_signal sig_p, sig_i, sig_d, sig_pi;
};

#endif