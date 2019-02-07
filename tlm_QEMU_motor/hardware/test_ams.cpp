#include "test_ams.h"

#include<iostream>

ref::ref(sc_core::sc_module_name name){
    std::cout << "BUILDING SC AMS MODULE" << std::endl;
}

void ref::set_attributes(){
    set_timestep(5, sc_core::SC_NS);
}

void ref::processing(){
    std::cout << "TEST HERE" << std::endl;
}