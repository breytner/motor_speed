#include "tdf2de.h"

Tdf2De::Tdf2De(sc_core::sc_module_name name)
    : sca_tdf::sca_module(name), x("x"), y("y") {
}

void Tdf2De::set_attributes(){
    int t0 = 1;
    // set_timestep(t0, sc_core::SC_MS);
    // x.set_rate(t0);
    // y.set_timestep(t0, sc_core::SC_MS); 
}

void Tdf2De::processing(){
    // y.write( x.read(x.get_rate()-1) );
    y.write(x.read());
}