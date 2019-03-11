#ifndef FIXED_POINT_PARAMS_H
#define FIXED_POINT_PARAMS_H

#include<stdint.h>

// Use either the FPU or fixed point arithmetics
#define USE_FLOATING_POINT 0

// Fixed point resolution
#define FIXED_POINT_N 10


// If the type defined here is changed, 
// thresholds in the avoid_saturation function must
// be changed correspondingly
typedef int32_t fixed_point_t;
typedef int64_t fixed_point_2x_t;


#endif