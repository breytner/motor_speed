#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include<systemc-ams>

SCA_TDF_MODULE(DcMotor){
    public:
        // Ports
        sca_tdf::sca_in<double> input_voltage;
        sca_tdf::sca_out<double> angular_speed;

        // Electromechanical system parameters
        double Jm, La, b, Ra, Kt, Ke;

        DcMotor(sc_core::sc_module_name name, 
            double Jm_=1.13e-2, double La_=1.0e-1, double b_=0.028,
            double Ra_=0.45, double Kt_=0.067, double Ke_=0.067);

        void set_attributes();
        void initialize();
        void processing();

    private: 
        // Transfer function definition
        // (Numerator and denominator coefficients)
        sca_util::sca_vector<double> num, den;

        // Laplace transfer funtion
        sca_tdf::sca_ltf_nd ltf_nd;
};

#endif