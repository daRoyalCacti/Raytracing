#pragma once

#include <functional>

#include "common.h"
#include "camera.h"
#include "hittable_list.h"
#include "scenes.h"

struct render_settings {
	double aspect_ratio = 16.0 / 9.0;
	unsigned image_width, image_height;
	unsigned samples_per_pixel;	//number of light rays per pixel
	unsigned max_depth = 50;	//max number of light bounces

	render_settings(const unsigned img_w, const unsigned samples = 100, const double aspect = 16.0 / 9.0) :
			image_width(img_w), samples_per_pixel(samples), aspect_ratio(aspect) {
		calc_height();
	}

	inline void calc_height() {
		image_height = static_cast<int>(image_width / aspect_ratio);
	}


	virtual void draw(const scene scn, std::function<color (const ray&, const hittable&, const int, const color&)> ray_color) {
		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = image_height-1; j>=0; --j) {
			std::cerr << "\rScanlines remaining: " << j+1 << " / " << image_height << std::flush;
			for (unsigned i = 0; i < image_width; ++i) {
				color pixel_color(0,0,0);
				#pragma omp parallel for
				for (int s = 0; s < samples_per_pixel; ++s) {
					auto u = double(i + random_double()) / (image_width-1);
					auto v = double(j + random_double()) / (image_height-1);
					ray r = scn.cam().get_ray(u, v);
					pixel_color += ray_color(r, scn.objects(), max_depth, scn.background());
				}
				write_color(std::cout, pixel_color, samples_per_pixel);
			}
		}
		std::cerr << "\r" << std::flush;
	}
};


struct low_render_settings : public render_settings {
	low_render_settings() : render_settings(400, 100) {}
};

struct high_render_settings : public render_settings {
	high_render_settings() : render_settings(1920, 500) {}
};
