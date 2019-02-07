#include "de2tdf.h"

De2Tdf::De2Tdf(sc_core::sc_module_name name)
    : x("x"), y("y") {
}

void De2Tdf::set_attributes(){
    int t0 = 1;
    // set_timestep(t0, sc_core::SC_MS);
    // y.set_timestep(1, sc_core::SC_MS);
    // y.set_rate(t0);
}

void De2Tdf::processing(){
    // for( unsigned i = 0; i < y.get_rate(); i++ ){
    //     y.write( x.read(), i);
    // }
    y.write(x.read());
    // std::cout << "de2tdf WROTE " << x.read() << " " << y.get_rate() << std::endl;
}