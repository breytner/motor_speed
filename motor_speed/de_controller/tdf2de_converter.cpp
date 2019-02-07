#include "tdf2de_converter.h"
#include "../common/constants.h"

tdf2de_converter::tdf2de_converter(sc_core::sc_module_name name)
    : x("x"), y("y") {
}

void tdf2de_converter::set_attributes(){
    set_timestep(DE_TIMESTEP, sc_core::SC_MS);
    x.set_rate(DE_RATE);
    y.set_timestep(DE_TIMESTEP, sc_core::SC_MS); 
}

void tdf2de_converter::processing(){
    y.write( x.read(x.get_rate()-1) );
}