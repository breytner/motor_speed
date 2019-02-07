#include"dc_motor_tdf.h"

dc_motor_tdf::dc_motor_tdf(sc_core::sc_module_name name, 
    double Jm_, double La_, double b_, double Ra_, double Kt_, double Ke_)
    : input_voltage("input_voltage"), angular_speed("angular"),
        Jm(Jm_), La(La_), b(b_), Ra(Ra_), Kt(Kt_), Ke(Ke_)
{}

void dc_motor_tdf::set_attributes(){
    angular_speed.set_delay(1);
    set_timestep(1, sc_core::SC_MS);
}

void dc_motor_tdf::initialize(){
    num(0) = Kt;
    den(0) = b * Ra + Kt * Ke;
    den(1) = Jm * Ra + b * La;
    den(2) = Jm * La;
}

void dc_motor_tdf::processing(){
    angular_speed.write( ltf_nd(num, den, input_voltage.read(), 1) );
}