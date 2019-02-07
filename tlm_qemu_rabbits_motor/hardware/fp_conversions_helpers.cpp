#include "fp_conversions_helpers.h"
#include<cmath>

// Conversion from float
fixed_point_t floatToFp(float x){
    fixed_point_t sign = x < 0.0 ? -1 : 1;
    return  sign * round( fabs(x) * pow(2, FIXED_POINT_N) );
}

// Conversion to float 
float fpToFloat(fixed_point_t x){ 
    return ( (float) x ) / pow(2, FIXED_POINT_N);
}
