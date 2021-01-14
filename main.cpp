#include "common.h"

#include "hittable_list.h"
#include "scenes.h"
#include "render.h"
#include "probability.h"
#include "fog.h"
#include "pdf.h"

#include <iostream>
#include <chrono>



color ray_color(const ray& r, const hittable& world, const int depth, const color& background, std::function<bool (const ray, const vec3, ray&)> hit_fog, shared_ptr<hittable> lights) {
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
	if (hit_fog(r, rec.p, scattered)) {
	    return fog_color * ray_color(scattered, world, depth-1, background, hit_fog, lights);
	}

	
	//else keep bouncing light
	color attenuation;
	double pdf_val;
	const color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);	//is black if the material doesn't emit



	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered, pdf_val))	//if the shouldn't scatter
		return emitted;

	const auto p0 = make_shared<hittable_pdf>(lights, rec.p);
	const auto p1 = make_shared<cosine_pdf>(rec.normal);
	mixture_pdf mixed_pdf(p0, p1);

	scattered = ray(rec.p, mixed_pdf.generate(), r.time());
	pdf_val = mixed_pdf.value(scattered.direction());



	return emitted + attenuation * ray_color(scattered, world, depth-1, background, hit_fog, lights) * rec.mat_ptr->scattering_pdf(r, rec, scattered) / pdf_val;	//return the color of the object darkened by the number of times the ray bounced

}


int main() {
	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cerr << "Computation started at " << std::ctime(&start_time);

	
	//the main drawing
	render_settings ren(600, 1000, 1, 1/*16.0f/9.0f*/);			//change this to change the quality of the render
    cornell_box_scene curr_scene(ren.aspect_ratio);	//change this to change the scene
	ren.draw(curr_scene, ray_color, no_fog);


	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cerr << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cerr << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
}

