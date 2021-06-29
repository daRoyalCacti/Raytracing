#pragma once

#include "common.h"

class camera {
	const point3 origin;    //camera is positioned
	const vec3 horizontal;    //defines the direction of horizontal through u, and how far horizontal to draw through viewport_width
	const vec3 vertical;      //defines the direction of vertical through v, and how far vertical to draw through viewport_height
    //const point3 lower_left_corner;
	const double lens_radius = 0;
	const double time0 = 0, time1 = 0;	//shutter open/close times

	const vec3 llc_m_o; // = lower_left_corner - origin
    // lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w
    // - w pushes the corner back to make the camera still have the origin at 'origin'
    // - focus_dist*w because this makes the light come from the lens of the camera
    // - not just the camera itself
    //thus, llc_m_o = - horizontal/2 - vertical/2 - focus_dist*w

    const vec3 w; //vector pointing from camera to point to look at
	const vec3 u; //vector pointing to the right -- orthogonal to the up vector and w
	const vec3 v; //the projection of the up vector on to the plane described by w (i.e. perpendicular to w)

	public:
	camera() = delete;

    //vfov := vertical field of view in degrees
    //lookfrom := position of the camera
    //lookat := point for the camera to look at
    //vup := defines the remaining tilt of the camera - the up vector for the camera
    //focus_dist := distance from the camera that is in focus
	camera(const point3 &lookfrom, const vec3 &lookat, const vec3 &vup, const double vfov, const double aspect_ratio, const double aperture, const double focus_dist,
			const double _time0, const double _time1) : time0(_time0), time1(_time1), lens_radius(aperture/2.0), origin(lookfrom), w(unit_vector(lookfrom - lookat)),
			//u(unit_vector(cross(vup, unit_vector(lookfrom - lookat)))), v(cross(unit_vector(lookfrom - lookat), unit_vector(cross(vup, unit_vector(lookfrom - lookat))))),
			u(unit_vector(cross(vup, w))), v(cross(w, u)),
			horizontal(focus_dist * aspect_ratio* 2.0 * tan(degrees_to_radians(vfov)/2) * unit_vector(cross(vup, unit_vector(lookfrom - lookat))) ),
			vertical(focus_dist * 2.0 * tan(degrees_to_radians(vfov)/2) * cross(unit_vector(lookfrom - lookat), unit_vector(cross(vup, unit_vector(lookfrom - lookat)))) ),
			llc_m_o(- horizontal/2 - vertical/2 - focus_dist*unit_vector(lookfrom - lookat)){

        /*u(unit_vector(cross(vup, w))), v(cross(w, u)),
          horizontal(focus_dist * aspect_ratio* 2.0 * tan(degrees_to_radians(vfov)/2) * unit_vector(cross(vup, w)) ),
                vertical(focus_dist * 2.0 * tan(degrees_to_radians(vfov)/2) * cross(w, u) ),
                lower_left_corner(origin - horizontal/2 - vertical/2 - focus_dist*unit_vector(lookfrom - lookat) ) {*/

		//const auto theta = degrees_to_radians(vfov);
		//const auto h = tan(theta/2);

		//const auto viewport_height = 2.0 * h;
		//const auto viewport_width = aspect_ratio*viewport_height;

		//w = unit_vector(lookfrom - lookat);
		//u = unit_vector(cross(vup, w));
		//v = cross(w, u);
		
		//uses a lense
		//origin = lookfrom;
		//horizontal = focus_dist * viewport_width * u;
		//vertical = focus_dist * viewport_height * v;
		//lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;


		//lens_radius = aperture/2;
	}

	[[nodiscard]] ray get_ray(const double s, const double t) const {
		//uses the thin lens approximation to generate depth of field
		const vec3 rd = lens_radius * random_in_unit_disk();	//randomness is required to get the blur
		const vec3 offset = u * rd.x() + v*rd.y();		//offset for where the light is coming from

		return ray(origin + offset, llc_m_o + s*horizontal + t*vertical - offset, random_double(time0, time1));	//random time to simulate motion blur
	}
};
