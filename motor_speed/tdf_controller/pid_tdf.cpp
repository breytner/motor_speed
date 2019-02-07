#include "pid_tdf.h"


pid_tdf::pid_tdf( sc_core::sc_module_name name, double kp_, double ki_, double kd_ ) 
    : e("e"), u("u"), kp(kp_), ki(ki_), kd(kd_) 
{

}

void pid_tdf::initialize(){
    num(0) = ki;
    num(1) = kp;
    num(2) = kd;
    den(0) = 0;
    den(1) = 1;
}

void pid_tdf::processing(){
    u.write( ltf_nd(num, den, e.read(), 1) );
}