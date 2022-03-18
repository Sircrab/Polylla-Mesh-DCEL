#ifndef UTILS_HPP
#define UTILS_HPP
#include <cmath>

double cross(const double& x1, const double& y1, const double& x2, const double& y2) {
    return x1 * y2 - x2 * y1;
}

#endif