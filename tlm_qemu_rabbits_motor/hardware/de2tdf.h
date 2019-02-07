#ifndef DE2TDF_H
#define DE2TDF_H

#include<systemc>
#include<systemc-ams>


SCA_TDF_MODULE(De2Tdf){
    public: 
        // Ports
        sca_tdf::sca_de::sca_in<double> x;
        sca_tdf::sca_out<double> y;

        SC_CTOR(De2Tdf);

        void set_attributes();
        void processing();
};

#endif