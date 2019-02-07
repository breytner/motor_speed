#ifndef DE2TDF_CONVERTER_H
#define DE2TDF_CONVERTER_H

#include<systemc>
#include<systemc-ams>


SCA_TDF_MODULE(de2tdf_converter){
    public: 
        // Ports
        sca_tdf::sca_de::sca_in<double> x;
        sca_tdf::sca_out<double> y;

        SC_CTOR(de2tdf_converter);

        void set_attributes();
        void processing();
};

#endif