#include "common.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "scenes.h"
#include "render.h"

#include <iostream>
#include <chrono>


color ray_color(const ray& r, const hittable& world, const int depth, const color& background) {
	//collision with any object
	hit_record rec;

	//If we've reach the bounce limit, no more light is gathered
	if (depth <= 0)
		return color(0,0,0);
	
	//If the ray hits nothing, return the background color
	if (!world.hit(r, 0.001, infinity, rec))
		return background;
	
	//else keep bouncing light
	ray scattered;
	color attenuation;
	const color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);	//is black if the material doesn't emit

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))	//if the shouldn't scatter
		return emitted;

	return emitted + attenuation * ray_color(scattered, world, depth-1, background);	//return the color of the object darkened by the number of times the ray bounced

}

int main() {
	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cerr << "Computation started at " << std::ctime(&start_time);

	
	//the main drawing
	render_settings ren(300, 200, 1.0);			//change this to change the quality of the render
	big_scene2 curr_scene(ren.aspect_ratio);	//change this to change the scene
	ren.draw(curr_scene, ray_color);


	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cerr << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cerr << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
}

