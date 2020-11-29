#pragma once

#include "common.h"
#include "hittable.h"

struct moving_sphere : public hittable {
	point3 center0, center1;	//centres of spheres at time0 and time1
	double time0, time1;
	double radius;
	shared_ptr<material> mat_ptr;

	moving_sphere();
	moving_sphere(const point3 cen0, const point3 cen1, const double _time0, const double _time1, const double r, const shared_ptr<material> m) :
		center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const override;

	virtual bool bounding_box(const double _time0, const double _time1, aabb& output_box) const override;

	inline point3 center(const double time) const {
		return center0 + (time - time0) / (time1 - time0) * (center1 - center0);
	}
};


bool moving_sphere::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
	//essentially the same as sphere::hit but center is now center(time)
	const vec3 oc = r.origin() - center(r.time());
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
	const vec3 outward_normal = (rec.p - center(r.time())) / radius;	//a normal vector is just a point on the sphere less the center
								//dividing by radius to make it normalised
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;	//the ray collides with the sphere
	
}

bool moving_sphere::bounding_box(const double _time0, const double _time1, aabb& output_box) const {
	aabb box0(center(_time0) - vec3(radius,radius,radius), center(_time0) + vec3(radius,radius,radius));
	aabb box1(center(_time1) - vec3(radius,radius,radius), center(_time1) + vec3(radius,radius,radius));
	output_box = surrounding_box(box0, box1);
	return true;
}
