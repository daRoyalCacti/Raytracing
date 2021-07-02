#include "render.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <string>


int main() {

	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << "Computation started at " << std::ctime(&start_time);



    scene weekend = 	big_scene1();
    render<600, 400> ren_weekend(weekend);
    ren_weekend.draw_on_convergence<100>("timing_test.png", 0.001);




	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
	return 0;

}

