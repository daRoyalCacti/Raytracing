#include "common.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "scenes.h"

#include <iostream>
#include <chrono>


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
	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cerr << "Computation started at " << std::ctime(&start_time);

	//image
	const auto aspect_ratio = 16.0 / 9.0;
	const unsigned image_width = 400;
	const unsigned image_height = static_cast<int>(image_width / aspect_ratio);
	const unsigned samples_per_pixel = 100;
	const unsigned max_depth = 50;	//max number of light bounces
	
	//World
	const auto world = two_perlin_spheres_scene();

	//Camera 
	const auto cam = two_perlin_spheres_scene_cam(aspect_ratio);

	//Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height-1; j>=0; --j) {
		std::cerr << "\rScanlines remaining: " << j+1 << " / " << image_height << std::flush;
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
	std::cerr << "\r" << std::flush;


	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cerr << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cerr << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
}

