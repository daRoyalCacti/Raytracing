#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

//common usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//common constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

//utility functions
inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	//Returns number from U[0,1)
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double random_double(const double min, const double max) {
	//Returns number from U[min, max)
	return min + (max-min)*random_double();
}

inline int random_int(const int min, const int max) {
	//Returns a random integer from U[min,  max]
	return static_cast<int>(random_double(min, max+1));
}

inline double clamp(const double x, const double min, const double max) {
	//forcing x to be in [min, max]
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

//common headers
#include "ray.h"
#include "vec3.h"
