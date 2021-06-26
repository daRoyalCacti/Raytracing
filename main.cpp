#include "render.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <string>


int main() {

	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << "Computation started at " << std::ctime(&start_time);


	/*const scene scenes_to_render[] = {cornell_box_scene2_fog(0.01), cornell_box_scene2_fog(0.007), cornell_box_scene2_fog(0.015)};//.{big_scene1_fog()};
	constexpr unsigned image_widths[] = {1000, 1000, 1000};
	constexpr unsigned samples[] = {500, 500, 500};
	constexpr unsigned buffers[] = {4, 4, 4};


	const std::string file_names[] = {"fog_med.png", "fog_small.png", "fog_big.png"};*/

	/*const scene scenes_to_render[] = {crate_scene()};
	constexpr unsigned samples[] = {50};
	constexpr unsigned buffers[] = {10};


	const std::string file_names[] = {"test.png"};


	constexpr unsigned n = sizeof(samples) / sizeof(unsigned);

	for (unsigned i = 0; i < n; i++) {
	    render<200, 200> ren(scenes_to_render[i], samples[i], buffers[i]);
	    ren.draw(file_names[i]);
	}*/

	scene curr_scene = 	crate_scene();
    render<200, 200, 20> ren(curr_scene);
    ren.draw_on_convergence("0.05.png", 0.05);





	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
	return 0;

}

