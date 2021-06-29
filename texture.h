#pragma once

#include "common.h"

#include "perlin.h"
#include "stb_image_ne.h"

struct texture {
	[[nodiscard]] virtual color value(double u, double v, const point3& p) const = 0;
};

struct solid_color : public texture {
	solid_color() = delete;
	explicit solid_color(const color c) : color_value(c) {}
	solid_color(const double red, const double green, const double blue) : solid_color(color(red, green, blue)) {}
	
	[[nodiscard]] color value(const double u, const double v, const vec3& p) const override {
		return color_value;
	}

	private:
	const color color_value;
};


struct checker_texture : public texture {
	const shared_ptr<texture> odd;
	const shared_ptr<texture> even;

	checker_texture() = delete;
	checker_texture(const shared_ptr<texture>& _even, const shared_ptr<texture>& _odd) : even(_even), odd(_odd) {}
	checker_texture(const color c1, const color c2) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	[[nodiscard]] color value(const double u, const double v, const point3& p) const override {
		const auto sines = sin(10*p.x()) * sin(10*p.y()) * sin(10*p.z());	//essentially a 4D sine wave

		if(sines < 0)	//sine is periodic. Having a different texture for if sine is positive or negative will given distinct regions of different colors
			return odd->value(u, v, p);
		else 
			return even->value(u, v, p);

	}
};


struct [[maybe_unused]] noise_texture : public texture {
	const perlin noise;
	const double scale;	//how detailed the noise is, bigger number := more noise

	noise_texture() = delete;
	explicit noise_texture(const double sc = 1.0) : scale(sc) {}

	[[nodiscard]] color value(const double u, const double v, const point3& p) const override {
		return color(1,1,1) *0.5 *(1.0 + noise.noise(scale*p));	//creates a gray color
									//needs to be scaled to go between 0 and 1 else the gamma correcting function will return NaN's
									// (sqrt of a negative number)
	}
};


struct turbulent_texture : public texture {
	const perlin noise;
	const double scale;	//how detailed the noise is, bigger number := more noise
	int depth;	//number of layers of noise

	turbulent_texture() = delete;
	explicit turbulent_texture(const double sc = 1.0, const int dpt = 7) : scale(sc), depth(dpt) {}
	
	[[nodiscard]] color value(const double u, const double v, const point3& p) const override {
		return color(1,1,1) * noise.turb(scale*p, depth);
	}
};


struct marble_texture : public texture {
	const perlin noise;
	const double scale;	//how detailed the noise is, bigger number := more noise

	marble_texture() = delete;
	explicit marble_texture(const double sc = 1.0) : scale(sc) {}
	
	[[nodiscard]] color value(const double u, const double v, const point3& p) const override {
		return color(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10* noise.turb(scale*p, 7) ) );
	}
};





class image_texture : public texture {
	unsigned char* data;	//the data read from file
	int width, height;	//the width and height of the image
	int bytes_per_scanline;

	public:
	const static int bytes_per_pixel = 3;

	image_texture() = delete;
	//image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	explicit image_texture(const char* filename) {
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);	//reading the data from disk

		if (!data) {	//file not read
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height =0;
		}
		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture() {
		delete data;
	}

	[[nodiscard]] color value(const double u, const double v, const vec3& p) const override {
		if (data == nullptr)	//if not texture data, return cyan color
			return color(0, 1, 1);

		//Clamp input texture coordinates to [0,1]^2
		const auto uu = clamp(u, 0.0, 1.0);
		const auto vv = 1.0 - clamp(v, 0.0, 1.0);	//Flip v to image coordinates

		auto i = static_cast<int>(uu*width);
		auto j = static_cast<int>(vv*height);

		//Clamp integer mapping since actual coordinates should be less than 1.0
		if (i >= width)  i = width - 1;
		if (j >= height) j = height - 1;

		const auto color_scale = 1.0 / 255.0;	//to scale the input from [0,255] to [0,1]
		const auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;	//the pixel at coordinates (u,v)

		return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
	}
};
