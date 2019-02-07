#ifndef TDF2DE_CONVERTER_H
#define TDF2DE_CONVERTER_H

#include<systemc>
#include<systemc-ams>

SCA_TDF_MODULE(tdf2de_converter){

    // Inputs
    sca_tdf::sca_in<double> x;

    // Output
    sca_tdf::sca_de::sca_out<double> y;

    SC_CTOR(tdf2de_converter);

    void set_attributes();

    void processing();

};

#endif