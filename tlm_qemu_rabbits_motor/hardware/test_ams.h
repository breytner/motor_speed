#ifndef REF_H
#define REF_H

#include<systemc>
#include<systemc-ams>

SCA_TDF_MODULE(ref) {
    public:
        ref(sc_core::sc_module_name name);
        void set_attributes();
        void processing();
};

#endif