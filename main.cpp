#include "timing_tests.hpp"

#include <iostream>
#include <chrono>


int main() {

    std::cout << "Initialising Halton sequence";
    const auto start_Halton = std::chrono::system_clock::now();
    global::Halton_rng.init();
    const auto end_Halton = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed_seconds_Halton = end_Halton - start_Halton;
    std::cout << " -- took : " << elapsed_seconds_Halton.count() << "s" << std::endl;

    //test2 test;
    //test.run();



	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << "Rendering started at " << std::ctime(&start_time);

	/*scene test = 	big_scene1_fog();
    render<600, 400> ren_test(test);
    ren_test.draw_on_convergence<100>("timing_test.png", 0.001);*/




	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";

	return 0;

}

