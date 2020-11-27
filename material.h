#pragma once

#include "common.h"

struct hit_record;

struct material {
	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;	
};

struct lambertian : public material {
	color albedo;	
	
	lambertian(const color& a) : albedo(a) {}

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		auto scatter_direction = rec.normal + random_unit_vector();
		
		//Catch degenerate scattering direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}
};


struct metal : public material {
	color albedo;
	double fuzz;	//how much light spreads out on collison
			//fuzz = 0 for perfect reflections
			//fuzz = 1 for very fuzzy reflections

	metal(const color& a, const double f = 0) : albedo(a), fuzz(f) {};

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);	//the incomming ray reflected about the normal
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());	//the scattered ray
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);	//making sure scattering not away fron the normal
	}
};
