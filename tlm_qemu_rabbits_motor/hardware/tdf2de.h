#ifndef TDF2DE_H
#define TDF2DE_H

#include<systemc>
#include<systemc-ams>

SCA_TDF_MODULE(Tdf2De){

    // Inputs
    sca_tdf::sca_in<double> x;

    // Output
    sca_tdf::sca_de::sca_out<double> y;

    SC_CTOR(Tdf2De);

    void set_attributes();

    void processing();

};

#endif