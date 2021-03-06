#pragma once

#include "ray.hpp"
#include "Halton.hpp"
#include "helpful.hpp"

/*=================================================================
 can be made better https://pbr-book.org/3ed-2018/Camera_Models/Realistic_Cameras (i.e. TODO)
 ========================================================*/
class camera {
	const point3 origin;    //camera is positioned
	const vec3 horizontal;    //defines the direction of horizontal through u, and how far horizontal to draw through viewport_width
	const vec3 vertical;      //defines the direction of vertical through v, and how far vertical to draw through viewport_height
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

	size_t halton_counter = 0, halton_counter2 = 0;  //used to get pseudo-random numbers from the Halton sequence

	public:
	camera() = delete;

    //vfov := vertical field of view in degrees
    //lookfrom := position of the camera
    //lookat := point for the camera to look at
    //vup := defines the remaining tilt of the camera - the up vector for the camera
    //focus_dist := distance from the camera that is in focus
	camera(const point3 &lookfrom, const vec3 &lookat, const vec3 &vup, const double vfov, const double aspect_ratio, const double aperture, const double focus_dist,
			const double _time0, const double _time1) : time0(_time0), time1(_time1), lens_radius(aperture/2.0), origin(lookfrom), w(unit_vector(lookfrom - lookat)),
			u(unit_vector(cross(vup, w))), v(cross(w, u)),
			horizontal(focus_dist * aspect_ratio* 2.0 * tan(degrees_to_radians(vfov)/2) * unit_vector(cross(vup, unit_vector(lookfrom - lookat))) ),
			vertical(focus_dist * 2.0 * tan(degrees_to_radians(vfov)/2) * cross(unit_vector(lookfrom - lookat), unit_vector(cross(vup, unit_vector(lookfrom - lookat)))) ),
			llc_m_o(- horizontal/2 - vertical/2 - focus_dist*unit_vector(lookfrom - lookat)){}

	[[nodiscard]] inline ray get_ray(const double s, const double t) {
		//uses the thin lens approximation to generate depth of field
		const vec3 rd = lens_radius * halton_random_in_unit_disk(halton_counter);	//randomness is required to get blur (i.e. depth of field)
		const vec3 offset = u * rd.x() + v*rd.y();		//offset for where the light is coming from

		return ray(origin + offset, llc_m_o + s*horizontal + t*vertical - offset, random_halton_1D(time0, time1, halton_counter2));	//random time to simulate motion blur
	}
};
