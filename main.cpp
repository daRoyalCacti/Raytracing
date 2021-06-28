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


    /*scene weekend = 	big_scene1();
    render<600, 400> ren_weekend(weekend);
    ren_weekend.draw_on_convergence<100>("speed_test_1.png", 0.0001);*/




    scene weekend = 	big_scene1();
    //render<7680, 4320, 100> ren_weekend(weekend);
    render<15360, 8640> ren_weekend(weekend);
    ren_weekend.draw_on_convergence<100>("raytracing_in_one_weekend_main_image.png", 0.001);

    scene weekend_fog = 	big_scene1_fog();
    render<1920, 1080> ren_weekend_fog(weekend_fog);
    ren_weekend_fog.draw_on_convergence<50>("raytracing_in_one_weekend_main_image_foggy.png", 0.01);

    scene cornell2 = 	cornell_box_scene2();
    render<1200, 1200> ren_cornell2(cornell2);
    ren_cornell2.draw_on_convergence<100>("cornell_box_sphere.png", 0.01);

    scene cornell = 	cornell_box_scene();
    render<1200, 1200> ren_cornell(cornell);
    ren_cornell.draw_on_convergence<100>("cornell_box.png", 0.01);

    //only if time
    ren_weekend_fog.draw_on_convergence<50>("raytracing_in_one_weekend_main_image_foggy_2.png", 0.001);

    ren_cornell2.draw_on_convergence<100>("cornell_box_sphere_2.png", 0.001);

    ren_cornell.draw_on_convergence<100>("cornell_box_2.png", 0.001);

    ren_weekend.draw_on_convergence<100>("raytracing_in_one_weekend_main_image_2.png", 0.0001);

    /*scene fog_box = 	cornell_box_scene2_fog();
    render<1200, 1200, 50> ren_fog(fog_box);
    ren_fog.draw_on_convergence("conell_box_foggy.png", 0.001);*/




	//end timing
	const auto end = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	std::cout << "Computation ended at " << std::ctime(&end_time);
	
	const std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << "s  or  " << elapsed_seconds.count() / 60.0 << "m  or  " << elapsed_seconds.count() / (60.0 * 60.0) << "h\n";
	return 0;

}

