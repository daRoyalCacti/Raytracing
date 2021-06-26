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

void write_color(std::ostream &out, const color pixel_color, const unsigned samples_per_pixel) {
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

//T was originally std::vector<std::vector<color>>
template<typename T>
inline void write_buffer(const std::string &file_name, const T buffer, const unsigned  samples_per_pixel) {
    std::ofstream out;
    out.open(file_name.c_str());

    const unsigned image_height = buffer[0].size();
    const unsigned image_width = buffer.size();


    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = (int)image_height-1; j>=0; --j) {
        for (unsigned i = 0; i < image_width; ++i) {
            write_color(out, buffer[i][j], samples_per_pixel);
        }
    }

    out.close();
}

namespace fs = std::filesystem;

//T was originally std::vector<std::vector<color>>
template<typename T>
inline void write_buffer_png(const std::string &file_name, const T buffer) {
    const auto img_w = buffer[0].size();
    const auto img_h = buffer.size();

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





//could be updated using std::string.find(std::string)
void average_images(const std::string &file_dir, const std::string &output_loc) {
	int num_files = 0;
	bool first_file = true;
	int img_w;
	int img_h;
	std::vector<color> fb;
	for (const auto & entry : fs::directory_iterator(file_dir) ) {	//itterating through all files in file_dir
		num_files++;
		int fb_counter = 0;

		std::string line;
		std::ifstream input(entry.path());	//reading in each file in file_dir
		if (input.is_open() ) {	
			int line_counter = 0;
			while (getline (input,line)) {	//reading in each line in the file
				line_counter++;

				if (first_file && line_counter == 2) {	//this line holds the image dimensions
					//setting img_w and img_h
					std::string width;
					std::string height;

					int size_of_width = 0;	//need to know how many digits in width. e.g. 1200 vs 120
					for (const auto& chara : line) {
						size_of_width++;
						if (isspace(chara)) {
							size_of_width--;
							break;
						}
					}

					width.append(line, 0, size_of_width);
					height.append(line, size_of_width+1, size_of_width+10);	//making size of height large to account for the case of small width and large height
					img_w = std::stoi(width);
					img_h = std::stoi(height);

					fb.resize(img_w*img_h);
				}

				if (line_counter > 3) {	//the ppm files have 3 lines of data that is not the image itself
					std::string col1;
					std::string col2;
					std::string col3;

					//finding space 1
					int space1 = 0;
					for (const auto& chara : line) {
						space1++;
						if (isspace(chara)) {
							space1--;
							break;
						}
					}

					//finding space 2
					int space2 = 0;
					for (const auto& chara : line) {
						space2++;
						if (space2 > (space1+1) && isspace(chara)) {
							space2--;
							break;
						}
					}

					col1.append(line, 0, space1);
					col2.append(line, space1+1, space2-space1);
					col3.append(line, space2+1, 3);

					color col((float)std::stoi(col1)*(float)std::stoi(col1)/(255.0f*255.0f), (float)std::stoi(col2)*(float)std::stoi(col2)/(255.0f*255.0f),
                              (float)std::stoi(col3)*(float)std::stoi(col3)/(255.0f*255.0f) );

					fb[fb_counter++] += col;

				}

			}

		}

		first_file = false;

		input.close();
	}

	//https://www.nongnu.org/pngpp/doc/0.2.9/
	png::image<png::rgb_pixel> image(img_w, img_h);
	for (int j = 0; j<img_h; j++) 
		for (int i = 0; i <img_w; i++) {
			const size_t pixel_index = j*img_w + i;

			double r = fb[pixel_index].x();
			double g = fb[pixel_index].y();
			double b = fb[pixel_index].z();

			r /= num_files;
			g /= num_files;
			b /= num_files;

			//gamma correcting using "gamma 2"
			//i.e. raising the color to the power of 1/gamma = 1/2
			r = sqrt(r);
			g = sqrt(g);
			b = sqrt(b);

			//color scaled to be in [0, 255]
			int r_w = static_cast<int>(256 * clamp(r, 0, 0.999));
			int g_w = static_cast<int>(256 * clamp(g, 0, 0.999));
			int b_w = static_cast<int>(256 * clamp(b, 0, 0.999));

			image[j][i] = png::rgb_pixel(r_w,g_w,b_w);
		}

	image.write(output_loc);

}
