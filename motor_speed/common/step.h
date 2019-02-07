#ifndef STEP_H
#define STEP_H

#include<systemc-ams>

SCA_TDF_MODULE(step) {
    public: 
        // Step amplitude
        double a;
        sca_tdf::sca_out<double> y;

        step( sc_core::sc_module_name name, double a_ = 1.0 ) 
        : a(a_) {

        }

        // [] rate, tstep, delay
        void set_attributes()
        {
            // time between activations
            set_timestep(1, sc_core::SC_MS); 
        }
        
        // behavior 
        void processing(){
            //const double t = get_time().to_seconds();
            y.write(a);
        }

};

#endif