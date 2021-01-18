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


	const scene scenes_to_render[] = {big_scene2()};//.{big_scene1_fog()};
	constexpr unsigned image_widths[] = {1000};
	constexpr unsigned samples[] = {1000};
	constexpr unsigned buffers[] = {10};
	const std::string file_names[] = {"image.png"};


	constexpr unsigned n = sizeof(image_widths) / sizeof(unsigned);

	for (unsigned i = 0; i < n; i++) {
	    render ren(scenes_to_render[i], image_widths[i], samples[i], buffers[i]);
	    ren.draw(file_names[i]);
	}


/*
	//the main drawing
	render ren(cornell_box_scene2_fog(),600, 5, 1);			//change this to change the quality of the render
	ren.draw();
 */



	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
	return 0;
}

