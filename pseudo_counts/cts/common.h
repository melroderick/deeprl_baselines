//
// Created by Mel Roderick on 8/9/17.
//

#include "fastmath.hpp"
#include <cmath>

#define UseFastJacobianLog false

inline double log_add(double log_x, double log_y) {
#if UseFastJacobianLog
    if (log_x < log_y) {
        return fast_jacoblog(log_y - log_x) + log_x;
    }
    return fast_jacoblog(log_x - log_y) + log_y;
#else
    if (log_x < log_y) {
        return fast_log(1.0 + exp(log_y - log_x)) + log_x;
    }
    return fast_log(1.0 + exp(log_x - log_y)) + log_y;
#endif
}