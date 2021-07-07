//
// Created by jacob on 6/7/21.
//

#ifndef RAYTRACER_TIMING_TESTS_HPP
#define RAYTRACER_TIMING_TESTS_HPP

#include "render.hpp"

template <size_t image_width, size_t image_height, size_t num_runs, size_t num_samples>
struct timing_test {
    scene sc;
    render<image_width, image_height> ren;

    std::vector<std::vector<color>> buffer;
    std::vector<std::vector<size_t>> samples;
    std::array<double, num_runs> times;

    timing_test() = delete;
    explicit timing_test(const scene &s) : ren(s), sc(s) {
        if (!global::Halton_rng.is_initialised) {
            std::cout << "Initialising Halton sequence";
            const auto start_Halton = std::chrono::system_clock::now();
            global::Halton_rng.init();
            const auto end_Halton = std::chrono::system_clock::now();
            const std::chrono::duration<double> elapsed_seconds_Halton = end_Halton - start_Halton;
            std::cout << " -- took : " << elapsed_seconds_Halton.count() << "s" << std::endl;
        }

        buffer.resize(image_width);
        samples.resize(image_width);

        for (size_t i = 0; i < image_width; i++) {
            buffer[i].resize(image_height);
            samples[i].resize(image_height);
            for (size_t j = 0; j < image_height; j++) {
                buffer[i][j] = vec3(0,0,0); //doesn't matter, result is never used
                samples[i][j] = num_samples;
            }
        }
    }

    void run() {
        for (size_t i = 0; i < num_runs; i++) {
            //std::cerr << i << "\n";
            const auto start = std::chrono::high_resolution_clock::now();
            ren.draw_to_buffer(buffer, samples);
            const auto end = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsed = end - start;
            times[i] = elapsed.count();
            std::cout << times[i] << "\n";
        }

        const double res_mean = mean(times);
        const double res_std = st_dev(times, res_mean);
        const double res_min = min_arr(times);
        const double res_max = max_arr(times);
        const double res_median = median(times);
        const double res_total = sum(times);


        std::cout << "Time taken : " << res_total << "s\n";
        std::cout << "\tMean\t: " << res_mean << "s\n";
        std::cout << "\tStd\t: " << res_std << "s\n";
        std::cout << "\tMedian\t: " << res_median << "s\n";
        std::cout << "\tMin\t: " << res_min << "s\n";
        std::cout << "\tMax\t: " << res_max << "s\n";
    }
};

//quick and dirty
struct test1 : public timing_test<300, 200, 5, 100> {
    test1() : timing_test(big_scene1_fog()) {}
};



#endif //RAYTRACER_TIMING_TESTS_HPP
