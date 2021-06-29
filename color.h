#pragma once

#include "vec3.h"

#include <iostream>

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cctype> 	//for isspace()
#include <vector>

#include <png++/png.hpp>	//for writing pngs

[[maybe_unused]] void write_color(std::ostream &out, const color pixel_color, const unsigned samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	//Divide the color by the number of samples
	//the component of each color is added to at each iteration
	//so this acts as an average
	const auto scale = 1.0 / samples_per_pixel;
	//gamma correcting using "gamma 2"
	//i.e. raising the color the power of 1/gamma = 1/2
	r = sqrt(scale * r);
	b = sqrt(scale * b);
	g = sqrt(scale * g);

	//Write the color
	//color is scaled to be in [0, 255]
	out << static_cast<int>(256 * clamp(r, 0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0, 0.999)) << '\n';
}

namespace fs = std::filesystem;

//T was originally std::vector<std::vector<color>>
template<typename T>
inline void write_buffer_png(const std::string &file_name, const T &buffer) {
    const auto img_w = buffer.size();
    const auto img_h = buffer[0].size();

    //https://www.nongnu.org/pngpp/doc/0.2.9/
    png::image<png::rgb_pixel> image(img_w, img_h);
    for (size_t j = 0; j<img_h; j++)
    //for (int j = (int)img_h - 1; j>=0; j--)
        for (size_t i = 0; i <img_w; i++) {

            //gamma correcting using "gamma 2"
            //i.e. raising the color to the power of 1/gamma = 1/2
            const double r = sqrt(buffer[i][j].x());
            const double g = sqrt(buffer[i][j].y());
            const double b = sqrt(buffer[i][j].z());

            //color scaled to be in [0, 255]
            const int r_w = static_cast<int>(256 * clamp(r, 0, 0.999));
            const int g_w = static_cast<int>(256 * clamp(g, 0, 0.999));
            const int b_w = static_cast<int>(256 * clamp(b, 0, 0.999));

            image[img_h-j-1][i] = png::rgb_pixel(r_w,g_w,b_w);
        }

    image.write(file_name);
}