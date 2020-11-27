#pragma once

#include "common.h"

class camera {
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;

	public:
	camera(const point3 lookfrom, const vec3 lookat, const vec3 vup, const double vfov, const double aspect_ratio) {	
		//vfov := vertical field of view in degrees
		//lookfrom := position of the camera
		//lookat := point for the camera to look at
		//vup := defines the remaining tilt of the camera - the up vector for the camera

		const auto theta = degrees_to_radians(vfov);
		const auto h = tan(theta/2);

		const auto viewport_height = 2.0 * h;
		const auto viewport_width = aspect_ratio*viewport_height;

		const auto w = unit_vector(lookfrom - lookat);	//vector pointing from camera to point to look at
		const auto u = unit_vector(cross(vup, w));	//vector pointing to the right -- orthogonal to the up vector and w
		const auto v = cross(w, u);			//the projection of the up vector on to the plane described by w (i.e. perpendicular to w)

		origin = lookfrom;	//camera is positioned at 'lookfrom'
		horizontal = viewport_width * u;	//defines the direction of horizontal through u, and how far horizontal to draw through viewport_width
		vertical = viewport_height * v;		//defines the direction of vertical through v, and how far vertical to draw through viewport_height
		lower_left_corner = origin - horizontal/2 - vertical/2 - w;	//w pushes the corner back to make the camera still have the origin at 'origin'
	}

	ray get_ray(const double s, const double t) const {
		return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
	}
};
