#ifndef CONSTANTS_H
#define CONSTANTS_H

#define REF_VAL 1.0
#define KP 3.0
#define KI 15.0
#define KD 0.3

#define MAX_SIM_TIME 1400 // Maximum simulation time in milliseconds

#define T0 1 // Base timestep in milliseconds (used in motor model)
#define DE_RATE 5 // Relative to T0
#define DE_TIMESTEP DE_RATE*T0

#define SIMULATION_TIME_MS 1400

#endif