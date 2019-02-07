#ifndef TDF_CONTROLLER_H
#define TDF_CONTROLLER_H

#include<systemc>
#include<systemc-ams>
#include "subs_tdf.h"
#include "pid_tdf.h"
#include "../common/step.h"

SC_MODULE(tdf_controller){

    public:
        // Plant output signal
        sca_tdf::sca_in<double> y;

        // Control signal
        sca_tdf::sca_out<double> u;

        tdf_controller(sc_core::sc_module_name name, double, double, double, double);

    private:        
        step ref_source;
        subs_tdf subs;
        pid_tdf tdf_pid;

        sca_tdf::sca_signal<double> ref_sig, err_sig;

};

#endif