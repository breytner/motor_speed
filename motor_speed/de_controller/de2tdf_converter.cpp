#include "de2tdf_converter.h"
#include "../common/constants.h"

de2tdf_converter::de2tdf_converter(sc_core::sc_module_name name)
    : x("x"), y("y") {
}

void de2tdf_converter::set_attributes(){
    set_timestep(DE_TIMESTEP, sc_core::SC_MS);
    y.set_timestep(T0, sc_core::SC_MS);
    y.set_rate(DE_RATE);
}

void de2tdf_converter::processing(){
    for( unsigned i = 0; i < y.get_rate(); i++ ){
        y.write( x.read(), i);
    }
}