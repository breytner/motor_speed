#ifndef LSF_CONTROLLER_H
#define LSF_CONTROLLER_H

#include<systemc>
#include<systemc-ams> 
#include"pid_lsf.h"

SC_MODULE(lsf_controller){
    public:
        // Plant output signal
        sca_tdf::sca_in<double> y;
        // Control signal
        sca_tdf::sca_out<double> u;

        // LSF-TDF interfaces
        sca_lsf::sca_tdf::sca_source y_conv;
        sca_lsf::sca_tdf::sca_sink u_conv;

        lsf_controller(sc_core::sc_module_name, double, double, double, double);       

    private: 
        sca_lsf::sca_source ref;
        sca_lsf::sca_sub subs; 
        pid_lsf lsf_pid;

        sca_lsf::sca_signal y_lsf_sig, u_lsf_sig, ref_sig, err_sig;

};
#endif