#pragma once

#include <functional>

#include "common.h"
#include "camera.h"
#include "hittable_list.h"
#include "scenes.h"

#include <fstream>

//for creating a directory
#include <bits/stdc++.h> 
#include <sys/stat.h> 
#include <sys/types.h> 


struct render_settings {
	double aspect_ratio = 16.0 / 9.0;
	unsigned image_width, image_height;
	unsigned samples_per_pixel;	//number of light rays per pixel
	unsigned max_depth = 50;	//max number of light bounces
	unsigned number_of_temp;

	render_settings(const unsigned img_w, const unsigned samples = 100, const unsigned temp = 10, const double aspect = 16.0 / 9.0) :
			image_width(img_w), samples_per_pixel(samples), number_of_temp(temp), aspect_ratio(aspect) {
		calc_height();
	}

	inline void calc_height() {
		image_height = static_cast<int>(image_width / aspect_ratio);
	}


	virtual void draw(const scene scn, std::function<color (const ray&, const hittable&, const int, const color&)> ray_color) {
		//creating a temp directory
		std::string rand_str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		std::random_device rd;	
		std::mt19937 generator(rd());

		std::shuffle(rand_str.begin(), rand_str.end(), generator);

		const std::string temp_file_dir = "./temp_image_dir_" + rand_str.substr(0, 15);

		mkdir(temp_file_dir.c_str(), 0777);

		for (int n = 0; n < number_of_temp; n++) {
			std::ofstream out;
			out.open(temp_file_dir + "/" + std::to_string(n) + ".ppm");


			out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

			for (int j = image_height-1; j>=0; --j) {
				std::cerr << "\rScanlines remaining: " << j+1 << " / " << image_height << " of image " << n+1 << " / " << number_of_temp << std::flush;
				for (unsigned i = 0; i < image_width; ++i) {
					color pixel_color(0,0,0);
					#pragma omp parallel for
					for (int s = 0; s < samples_per_pixel; ++s) {
						auto u = double(i + random_double()) / (image_width-1);
						auto v = double(j + random_double()) / (image_height-1);
						ray r = scn.cam().get_ray(u, v);
						pixel_color += ray_color(r, scn.objects(), max_depth, scn.background());
					}
					write_color(out, pixel_color, samples_per_pixel);
				}
			}
			out.close();
			std::cerr << "\r" << std::flush;
		}

		average_images(temp_file_dir, "image.png");

		//deleting the temp images
		std::filesystem::path pathToDelete(temp_file_dir);
		remove_all(pathToDelete);
	}
};


struct low_render_settings : public render_settings {
	low_render_settings() : render_settings(400, 100) {}
};

struct high_render_settings : public render_settings {
	high_render_settings() : render_settings(1920, 500) {}
};
