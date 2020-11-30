#pragma once

#include "common.h"

#include "perlin.h"

struct texture {
	virtual color value(const double u, const double v, const point3& p) const = 0;
};

struct solid_color : public texture {
	solid_color() {}
	solid_color(const color c) : color_value(c) {}
	solid_color(const double red, const double green, const double blue) : solid_color(color(red, green, blue)) {}
	
	virtual color value(const double u, const double v, const vec3& p) const override {
		return color_value;
	}

	private:
	color color_value;
};


struct checker_texture : public texture {
	shared_ptr<texture> odd;
	shared_ptr<texture> even;

	checker_texture() {}
	checker_texture(const shared_ptr<texture> _even, const shared_ptr<texture> _odd) : even(_even), odd(_odd) {}
	checker_texture(const color c1, const color c2) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	virtual color value(const double u, const double v, const point3& p) const override {
		const auto sines = sin(10*p.x()) * sin(10*p.y()) * sin(10*p.z());	//essentially a 4D sine wave

		if(sines < 0)	//sine is periodic. Having a different texture for if sine is positive or negative will given distinct regions of different colors
			return odd->value(u, v, p);
		else 
			return even->value(u, v, p);

	}
};


struct noise_texture : public texture {
	perlin noise;
	double scale = 1.0;

	noise_texture() {}
	noise_texture(const double sc) : scale(sc) {}

	virtual color value(const double u, const double v, const point3& p) const override {
		return color(1,1,1) *0.5 *(1.0 + noise.noise(scale*p));	//creates a gray color
									//needs to be scaled to go between 0 and 1 else the gamma correcting function will return NaN's
									// (sqrt of a negative number)
	}
};
