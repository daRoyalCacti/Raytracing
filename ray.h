#pragma once

#include "vec3.h"

struct ray {
	point3 orig;
	vec3 dir;
	double tm = 0;	//time the ray exists at
	
	ray() = default;
	ray(const point3& origin, const vec3& direction, const double time = 0.0) : orig(origin), dir(direction), tm(time) {}

	[[nodiscard]] inline point3 origin() const {return orig;}
	[[nodiscard]] inline vec3 direction() const {return dir;}
	[[nodiscard]] inline double time() const {return tm;}

	[[nodiscard]] point3 at(const double t) const {
		return orig + t*dir;
	}
};
