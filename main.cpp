#include "common.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "scenes.h"

#include <iostream>


color ray_color(const ray& r, const hittable& world, const int depth) {
	//collision with any object
	hit_record rec;

	//If we've reach the bounce limit, no more light is gathered
	if (depth <= 0)
		return color(0,0,0);
	//else keep bouncing light
	if (world.hit(r, 0.001, infinity, rec)) {	//if any object is hit
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))	//if the ray should scatter
			return attenuation * ray_color(scattered, world, depth-1);	//return the color of the object darkened by the number of times the ray bounced
		return color(0,0,0);
	}
	//background color
	const vec3 unit_direction = unit_vector(r.direction());
	const auto t = 0.5*(unit_direction.y() + 1.0);
	return (1.0-t)*color(1.0,1.0,1.0) + t*color(0.5,0.7, 1.0);
}

int main() {
	//image
	const auto aspect_ratio = 16.0 / 9.0;
	const unsigned image_width = 400;
	const unsigned image_height = static_cast<int>(image_width / aspect_ratio);
	const unsigned samples_per_pixel = 100;
	const unsigned max_depth = 50;	//max number of light bounces
	
	//World
	const auto world = random_scene();

	//Camera 
	const point3 lookfrom(13,2,3);
	const point3 lookat(0,0,0);
	const vec3 vup(0,1,0);
	const auto dist_to_focus = 10.0;
	const auto aperture = 0.1;
	const double fov = 20.0;

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

	//Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height-1; j>=0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (unsigned i = 0; i < image_width; ++i) {
			color pixel_color(0,0,0);
			#pragma omp parallel for
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = double(i + random_double()) / (image_width-1);
				auto v = double(j + random_double()) / (image_height-1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.\n";
}

