#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>


constexpr bool log_scanlines = false;  //prints the number of scanlines remaining when rendering an image



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
    if (!std::isfinite(x)) return min;
	//forcing x to be in [min, max]
	if (x < min) return min;
	if (x > max) return max;
	return x;
}




//common headers
#include "ray.h"
#include "vec3.h"


//https://external-content.duckduckgo.com/iu/?u=https%3A%2F%2Fi0.wp.com%2Fwww.therightgate.com%2Fwp-content%2Fuploads%2F2018%2F05%2Fspherical-cartesian-conversion.jpg%3Fresize%3D640%252C159%26ssl%3D1&f=1&nofb=1
[[maybe_unused]] inline vec3 cartesian_to_spherical(const vec3& input) {
    const double r = input.norm();
    const double theta = acos(input.z() / r);
    const double phi   = atan2(input.y(), input.x());

    return vec3(r, theta, phi);
}

[[maybe_unused]] inline vec3 spherical_to_cartesian(const vec3& input) {
    const double x = input[0]*sin(input[1])*cos(input[2]);
    const double y = input[0]*sin(input[1])*sin(input[2]);
    const double z = input[0]*cos(input[1]);

    return vec3(x, y ,z);
}