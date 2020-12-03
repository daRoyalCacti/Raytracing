#pragma once

#include "common.h"

#include "texture.h"

struct hit_record;

struct material {
	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;	
	virtual color emitted(const double u, const double v, const point3& p) const {
		return color(0, 0, 0);
	}
};

struct lambertian : public material {
	shared_ptr<texture> albedo;
	
	lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
	lambertian(const shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		auto scatter_direction = rec.normal + random_unit_vector();
		
		//Catch degenerate scattering direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, ray_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};


struct metal : public material {
	shared_ptr<texture> albedo;
	double fuzz;	//how much light spreads out on collison
			//fuzz = 0 for perfect reflections
			//fuzz = 1 for very fuzzy reflections

	metal(const color& a, const double f = 0) : albedo(make_shared<solid_color>(a)), fuzz(f) {}
	metal(const shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);	//the incomming ray reflected about the normal
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), ray_in.time());	//the scattered ray
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return (dot(scattered.direction(), rec.normal) > 0);	//making sure scattering not away fron the normal
	}
};


struct dielectric : public material {
	double ir;	//index of refraction of the material

	dielectric(const double index_of_refraction) : ir(index_of_refraction) {}

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		attenuation = color(1.0, 1.0, 1.0);	//material should be clear so white is a good choice for the color (absorbs nothing)
		const double refraction_ratio = rec.front_face ? (1.0/ir) : ir;	//refraction ratio = (refractive index of incident material) / (refrative index of transmitted material)
										//assuiming that the incident material is air, and the refractive index of air is 1
										//this means 'refractive index of incident material' = 1
										// - that is assuming the ray was initially in air and then collides with the object
										// - if the ray was in the object and collided with air 'refractive index of transmitted material' = 1
										//Therefore, if the ray is initially in air, refraction ratio = 1/ir
										//else refraction ratio = ir
										//if front_face is true, then the ray opposes the normal vector
										// - i.e. the light is moving towards the material
		const vec3 unit_direction = unit_vector(ray_in.direction());
		
		//implementing total internal reflectiion
		const double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		const double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
		
		const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 direction;
		
		//uses the Schlick approximation to give reflectivity that varies with angle
		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
			//Must reflect
			direction = reflect(unit_direction, rec.normal);
		} else {
			//Can Refract (taking it as always refracting)
			direction = refract(unit_direction, rec.normal, refraction_ratio);
		}

		scattered = ray(rec.p, direction, ray_in.time());
		return true;
	}

	private:
	static double reflectance(const double cosine, const double ref_idx) {
		//use Schlick's approximation for reflectance
		const auto sqrt_r0 = (1-ref_idx) / (1+ref_idx);
		const auto r0 = sqrt_r0 * sqrt_r0;
		return r0 + (1-r0) * pow(1-cosine, 5);
	}
};


struct diffuse_light : public material {
	shared_ptr<texture> emit;

	diffuse_light(const shared_ptr<texture> a) : emit(a) {}
	diffuse_light(const color c) : emit(make_shared<solid_color>(c)) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		return false;
	}

	virtual color emitted(const double u, const double v, const point3& p) const {
		return emit->value(u, v, p);
	}
};

//for scattering
struct isotropic : public material {
	shared_ptr<texture> albedo;

	isotropic(const color c) : albedo(make_shared<solid_color>(c)) {}
	isotropic(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		scattered = ray(rec.p, random_in_unit_sphere(), ray_in.time());	//pick a random direction for the ray to scatter
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};
