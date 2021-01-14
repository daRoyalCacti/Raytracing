#include "common.h"

#include "hittable_list.h"
#include "scenes.h"
#include "render.h"
#include "probability.h"

#include <iostream>
#include <chrono>

//https://www.astro.umd.edu/~jph/HG_note.pdf
//https://blog.demofox.org/2014/06/22/analytic-fog-density/
//https://computergraphics.stackexchange.com/questions/227/how-are-volumetric-effects-handled-in-raytracing
bool hit_fog(ray start, vec3 end, ray &scattered) {
    const double lambda = 0.05;  //determines how often particles scatter - 'density of the fog'
    const double g1 = 0.3, g2 = 0.4;   //determines how particles scatter
    const auto length = (start.orig-end).length();
    /*const auto prob = exp_cdf(length, lambda)

    if (rand_double() > prob)
        return false;

    const auto travelled = rand_double(0, length);*/

    const auto pos = rand_exp(lambda);

    if (pos > length)   //the ray should scatter after the ray has collided
        return false;

    const auto theta = rand_Henyey_Greensteing(g1);
    const auto phi = rand_Henyey_Greensteing(g2);
    scattered = ray(start.at(pos), vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(phi)), start.tm + pos);
    return true;
}

color ray_color(const ray& r, const hittable& world, const int depth, const color& background) {
	//collision with any object
	hit_record rec;

	const color fog_color = color(0.8, 0.8, 0.8);

	//If we've reach the bounce limit, no more light is gathered
	if (depth <= 0)
		return color(0,0,0);
	
	//If the ray hits nothing, return the background color
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

    ray scattered;
	/*if (hit_fog(r, rec.p, scattered)) {
	    return fog_color * ray_color(scattered, world, depth-1, background);
	}*/

	
	//else keep bouncing light
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
	render_settings ren(120, 10, 10, 16.0f/9.0f);			//change this to change the quality of the render
	big_scene1 curr_scene(ren.aspect_ratio);	//change this to change the scene
	ren.draw(curr_scene, ray_color);


	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cerr << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cerr << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
}

