#include "common.h"

#include "hittable_list.h"
#include "scenes.h"
#include "render.h"
#include "probability.h"
#include "fog.h"
#include "pdf.h"


#include <iostream>
#include <chrono>



int main() {
	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cerr << "Computation started at " << std::ctime(&start_time);

	
	//the main drawing
	render_settings ren(120, 100, 1, /*1*/16.0f/9.0f);			//change this to change the quality of the render
    big_scene1 curr_scene(ren.aspect_ratio);	//change this to change the scene
	ren.draw(curr_scene, ray_color_no_importance, no_fog);


	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cerr << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cerr << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
}

