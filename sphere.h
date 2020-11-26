#pragma once

#include "hittable.h"
#include "vec3.h"

struct sphere : public hittable {
	point3 center;
	double radius;

	sphere() {}
	sphere(const point3 cen, const double r): center(cen), radius(r) {}

	virtual bool hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const override;
};

bool sphere::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
	//using the quadratic equation to find if (and when) 'ray' collides with sphere centred at 'center' with radius 'radius'
	
	const vec3 oc = r.origin() - center;	
	const auto a = r.direction().length_squared();
	const auto half_b =  dot(oc, r.direction());	//uses half b to simplify the quadratic equation
	const auto c = oc.length_squared() - radius * radius;
	const auto discriminant = half_b*half_b - a*c;

	if (discriminant < 0) return false;	//if there is no collision
	const auto sqrtd = sqrt(discriminant);

	//Find the nearest root that lies in the acceptable range.
	auto root = (-half_b - sqrtd) / a;	//first root
	if (root < t_min || t_max < root) {	//if the first root is ouside of the accepctable range
		//if true, check the second root
		root = (-half_b - sqrtd) / a;
		if (root < t_min || t_max < root)
			//if the second root is ouside of the range, there is no hit
			return false;
	}

	//if the first root was accpetable, root is the first root
	//if the first root was not acceptable, root is the second root
	
	rec.t = root;	//root is finding time
	rec.p = r.at(rec.t);
	const vec3 outward_normal = (rec.p - center) / radius;	//a normal vector is just a point on the sphere less the center
								//dividing by radius to make it normalised
	rec.set_face_normal(r, outward_normal);

	return true;	//the ray collides with the sphere
}


