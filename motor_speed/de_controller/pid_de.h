#ifndef PID_DE_H
#define PID_DE_H

#include<systemc>

SC_MODULE(pid_de){
    SC_HAS_PROCESS(pid_de);
    
    public:
        // Error signal
        sc_core::sc_in<double> e;

        // Control signal
        sc_core::sc_out<double> u;

        // PID controller parameters
        double kp, ki, kd;

        // Processes
        void calc_control_signal();

        pid_de(sc_core::sc_module_name, 
            double kp_ = 1.0, 
            double ki_ = 0, 
            double kd_ = 0);

    private:
        double last_control, last_errors[2];

};
#endif