#pragma once

#include "ONB.hpp"
#include "pdf.hpp"
#include "texture.hpp"

struct hit_record;

struct scatter_record {
    ray specular_ray;
    bool is_specular;
    color attenuation;
    std::shared_ptr<pdf> pdf_ptr;
};

struct material {
	virtual bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) {
	    return false;
	}

	[[nodiscard]] virtual color emitted(const ray& r_in, const hit_record& rec, const double u, const double v, const point3& p) const {
		return color(0, 0, 0);
	}
	[[nodiscard]] virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
	    return 0;
	}
};

//idealisation of matte
struct lambertian : public material {
	const std::shared_ptr<texture> albedo;

	lambertian() = delete;
	explicit lambertian(const color& a) : albedo(std::make_shared<solid_color>(a)) {}
	explicit lambertian(std::shared_ptr<texture> a) : albedo(std::move(a)) {}

	bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) override {
		srec.is_specular = false;
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
		return true;
	}

	[[nodiscard]] inline double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
        const auto cosine = dot(rec.normal, unit_vector(scattered.direction()) );
        return cosine < 0 ? 0 : cosine/pi;
    }
};


struct metal : public material {
	const std::shared_ptr<texture> albedo;
	const double fuzz = 0;	//how much light spreads out on collision
			//fuzz = 0 for perfect reflections
			//fuzz = 1 for very fuzzy reflections
	size_t halton_counter = 0;

	metal() = delete;
	explicit metal(const color& a, const double f = 0) : albedo(std::make_shared<solid_color>(a)), fuzz(f) {}
	explicit metal(std::shared_ptr<texture> a, const double f = 0) : albedo(std::move(a)), fuzz(f) {}

	inline bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) override {
		const vec3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);	//the incoming ray reflected about the normal
		srec.is_specular = true;
		srec.specular_ray = ray(rec.p, reflected + fuzz * halton_random_in_unit_sphere(halton_counter), ray_in.time());	//the scattered ray
		//using Schlick's formula
		const auto unit_direction = unit_vector(ray_in.direction());
		const auto cosine = fmin(dot(-unit_direction, rec.normal), 1.0);
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p) + (color(1, 1, 1) - albedo->value(rec.u, rec.v, rec.p)) * pow(1-cosine, 5);
        srec.pdf_ptr = nullptr;
		return (dot(srec.specular_ray.direction(), rec.normal) > 0);	//making sure scattering not opposing the normal
	}
};


struct dielectric : public material {
	const double ir;	//index of refraction of the material
	size_t halton_counter = 0;

	dielectric() = delete;
	explicit dielectric(const double index_of_refraction) : ir(index_of_refraction) {}

	inline bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) override {
		const double refraction_ratio = rec.front_face ? (1.0/ir) : ir;	//refraction ratio = (refractive index of incident material) / (refractive index of transmitted material)
										//assuming that the incident material is air, and the refractive index of air is 1
										//this means 'refractive index of incident material' = 1
										// - that is assuming the ray was initially in air and then collides with the object
										// - if the ray was in the object and collided with air 'refractive index of transmitted material' = 1
										//Therefore, if the ray is initially in air, refraction ratio = 1/ir
										//else refraction ratio = ir
										//if front_face is true, then the ray opposes the normal vector
										// - i.e. the light is moving towards the material
		const vec3 unit_direction = unit_vector(ray_in.direction());
		
		//implementing total internal reflection
		const double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		const double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
		
		const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 direction;
		
		//uses the Schlick approximation to give reflectivity that varies with angle
		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_halton_1D(halton_counter)) {
			//Must reflect
			direction = reflect(unit_direction, rec.normal);
		} else {
			//Can Refract (taking it as always refracting)
			direction = refract(unit_direction, rec.normal, refraction_ratio);
		}

		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		srec.attenuation = color(1.0, 1.0, 1.0); //material should be clear so white is a good choice for the color (absorbs nothing)
		srec.specular_ray = ray(rec.p, direction, ray_in.time());
		return true;
	}

	private:
	static inline double reflectance(const double cosine, const double ref_idx) {
		//use Schlick's approximation for reflectance
		const auto sqrt_r0 = (1-ref_idx) / (1+ref_idx);
		const auto r0 = sqrt_r0 * sqrt_r0;
		return r0 + (1-r0) * pow(1-cosine, 5);
	}
};


struct diffuse_light : public material {
	const std::shared_ptr<texture> emit;

	diffuse_light() = delete;
	explicit diffuse_light(std::shared_ptr<texture> a) : emit(std::move(a)) {}
	explicit diffuse_light(const color c) : emit(std::make_shared<solid_color>(c)) {}

	bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) override {
		return false;
	}

	[[nodiscard]] inline color emitted(const ray& r_in, const hit_record& rec, const double u, const double v, const point3& p) const override {
	    if (rec.front_face)
		    return emit->value(u, v, p);
	    else
	        return color(0,0,0);
	}
};

//scatters evenly in all directions
struct isotropic : public material {
	const std::shared_ptr<texture> albedo;
	size_t halton_counter = 0;

	isotropic() = delete;
	explicit isotropic(const color c) : albedo(std::make_shared<solid_color>(c)) {}
	explicit isotropic(std::shared_ptr<texture> a) : albedo(std::move(a)) {}

	inline bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) override {
	    srec.is_specular = true;    //not sure
		srec.specular_ray = ray(rec.p, halton_random_in_unit_sphere(halton_counter), ray_in.time());	//pick a random direction for the ray to scatter
		srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = nullptr;
		return true;
	}
};

//scatters evenly in all directions
struct Henyey_Greensteing : public material {
    const std::shared_ptr<texture> albedo;
    Henyey_Greensteing_pdf henyeyGreensteingPdf;
    size_t halton_counter = 0;

    Henyey_Greensteing() = delete;
    explicit Henyey_Greensteing(const color c, double g = 0.5) : albedo(std::make_shared<solid_color>(c)), henyeyGreensteingPdf(g) {}
    explicit Henyey_Greensteing(std::shared_ptr<texture> a, double g = 0.5) : albedo(std::move(a)), henyeyGreensteingPdf(g) {}

    inline bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) override {
        srec.is_specular = true;    //not sure
        srec.specular_ray = ray(rec.p, henyeyGreensteingPdf.generate(ray_in.direction()), ray_in.time());	//pick a random direction for the ray to scatter
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = nullptr;
        return true;
    }
};
