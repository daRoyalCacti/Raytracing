#pragma once

#include "vec3.h"

struct ray {
	point3 orig;
	vec3 dir;
	double tm;	//time the ray exists at
	
	ray() {}
	ray(const point3& origin, const vec3& direction, const double time = 0.0) : orig(origin), dir(direction), tm(time) {}

	inline point3 origin() const {return orig;}
	inline vec3 direction() const {return dir;}
	inline double time() const {return tm;}

	point3 at(const double t) const {
		return orig + t*dir;
	}
};
