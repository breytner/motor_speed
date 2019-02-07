#ifndef SUBS_TDF_H
#define SUBS_TDF_H

#include<systemc-ams>

SCA_TDF_MODULE(subs_tdf){

    // Inputs
    sca_tdf::sca_in<double> x1, x2;

    // Output
    sca_tdf::sca_out<double> y;


    SC_CTOR(subs_tdf) : x1("x1"), x2("x2"), y("y") {
    }

    void set_attributes() {
        // y.set_delay(1);
    }
    
    void initialize() {}

    void processing(){
        y.write( x1.read() - x2.read() );
    }

    void ac_processing() {}

};

#endif