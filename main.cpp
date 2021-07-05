#include "render.hpp"

#include <iostream>
#include <chrono>


int main() {

	//start timing
	const auto start = std::chrono::system_clock::now();
	const std::time_t start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << "Computation started at " << std::ctime(&start_time);

	std::cout << "Initialising Halton sequence";
    const auto start_Halton = std::chrono::system_clock::now();
    global::Halton_rng.init();
    const auto end_Halton = std::chrono::system_clock::now();
    const std::chrono::duration<double> elapsed_seconds_Halton = end_Halton - start_Halton;
    std::cout << " -- took : " << elapsed_seconds_Halton.count() << "s" << std::endl;

    /*scene test = 	big_scene1();
    render<600, 400> ren_test(test);
    ren_test.draw_on_convergence<100>("timing_test.png", 0.001);*/


    /*scene scene1 = 	big_scene1_fog();
    render<1920, 1080> ren_scene1(scene1);
    ren_scene1.draw_on_convergence<100>("raytracing_in_one_weekend_main_image_foggy.png", 0.001);*/

    scene conell_box = 	cornell_box_scene2_smokey();
    render<1200, 1200> ren_box(conell_box);
    ren_box.draw_on_convergence<200>("cornell_box_smokey.png", 0.001);




	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
	return 0;

}

