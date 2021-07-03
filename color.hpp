#pragma once

#include "vec3.hpp"
#include <cmath>
#include <png++/png.hpp>	//for writing pngs

//writing a single color to some stream (mainly ppm files)
// - applies gamma 2 correction
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
	out << static_cast<int>(256 * std::clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * std::clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * std::clamp(b, 0.0, 0.999)) << '\n';
}


//writing an entire image (buffer) to some file location as a png using png++
// - applies gamma 2 correction

//T was originally std::vector<std::vector<color>>
template<typename T>
void write_buffer_png(const std::string &file_name, const T &buffer) {
    const auto img_w = buffer.size();
    const auto img_h = buffer[0].size();

    //https://www.nongnu.org/pngpp/doc/0.2.9/
    png::image<png::rgb_pixel> image(img_w, img_h);
    for (size_t j = 0; j<img_h; j++)
        for (size_t i = 0; i <img_w; i++) {

            //gamma correcting using "gamma 2"
            //i.e. raising the color to the power of 1/gamma = 1/2
            const double r = std::sqrt(buffer[i][j].x());
            const double g = std::sqrt(buffer[i][j].y());
            const double b = std::sqrt(buffer[i][j].z());

            //color scaled to be in [0, 255]
            const int r_w = static_cast<int>(256 * std::clamp(r, 0.0, 0.999));
            const int g_w = static_cast<int>(256 * std::clamp(g, 0.0, 0.999));
            const int b_w = static_cast<int>(256 * std::clamp(b, 0.0, 0.999));

            image[img_h-j-1][i] = png::rgb_pixel(r_w,g_w,b_w);
        }

    image.write(file_name);
}