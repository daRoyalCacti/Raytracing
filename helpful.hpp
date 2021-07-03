#pragma once

#include <limits>

//common constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;
constexpr double two_pi = 2*pi;

//utility functions
inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}
