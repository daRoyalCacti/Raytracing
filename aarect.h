//axis aligned rectangle
#pragma once

#include "common.h"

#include "hittable.h"

struct xy_rect : public hittable {
	shared_ptr<material> mp;
	double x0, x1, y0, y1, k;	//x's and y's define a rectangle in the standard way
					//k defines z position
	xy_rect() {}
	xy_rect(const double _x0, const double _x1, const double _y0, const double _y1, const double _k, const shared_ptr<material> mat)
		: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};
	
	virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const override;

	virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		//just padding the z direction by a small amount
		const double small = 0.0001;
		output_box = aabb(point3(x0, y0, k-small), point3(x1, y1, k+small));
		return true;
	}
};

//very similar to xy rect -- see for comments
struct xz_rect : public hittable {
	shared_ptr<material> mp;
	double x0, x1, z0, z1, k;	//k defines y position

	xz_rect() {}
	xz_rect(const double _x0, const double _x1, const double _z0, const double _z1, const double _k, const shared_ptr<material> mat)
		: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const override;
	
	virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		const double small = 0.0001;
		output_box = aabb(point3(x0, k-small, z1), point3(x1, k+small, z1));
		return true;
	}
};

//very similar to xy rect -- see for comments
struct yz_rect : public hittable {
	shared_ptr<material> mp;
	double y0, y1, z0, z1, k;	//k defines x position

	yz_rect() {}
	yz_rect(const double _y0, const double _y1, const double _z0, const double _z1, const double _k, const shared_ptr<material> mat)
		: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

	virtual bool hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const override;

	virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		const double small = 0.0001;
		output_box = aabb(point3(k-small, y0, z0), point3(k+small, y1, z1));
		return true;
	}
};


bool xy_rect::hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const {
	const auto t = (k-r.origin().z()) / r.direction().z();	//time of collision - see aabb.h for why this is the case

	if (t < t_min || t > t_max)	//if collision is outside of specified times
		return false;
	
	//the x and y components of the ray at the time of collision
	const auto x = r.origin().x() + t*r.direction().x();
	const auto y = r.origin().y() + t*r.direction().y();

	//the collision algorithm
	// - simply if the x and y components of the point of collision are outside the specified size of the rectangle
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;	//there was no collision
	
	//normalise x and y to be used as texture coords
	rec.u = (x-x0)/(x1-x0);
	rec.v = (y-y0)/(y1-y0);

	rec.t = t;

	const auto outward_normal = vec3(0, 0, 1);	//the trival normal vector
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);

	return true;
}



bool xz_rect::hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const {
	const auto t = (k-r.origin().y()) / r.direction().y();	//time of collision - see aabb.h for why this is the case

	if (t < t_min || t > t_max)	//if collision is outside of specified times
		return false;
	
	//the x and y components of the ray at the time of collision
	const auto x = r.origin().x() + t*r.direction().x();
	const auto z = r.origin().z() + t*r.direction().z();

	//the collision algorithm
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;	//there was no collision
	
	//normalise x and z to be used as texture coords
	rec.u = (x-x0)/(x1-x0);
	rec.v = (z-z0)/(z1-z0);

	rec.t = t;

	const auto outward_normal = vec3(0, 1, 0);	//the trival normal vector
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);

	return true;
}



bool yz_rect::hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const {
	const auto t = (k-r.origin().x()) / r.direction().x();	//time of collision - see aabb.h for why this is the case

	if (t < t_min || t > t_max)	//if collision is outside of specified times
		return false;
	
	//the x and y components of the ray at the time of collision
	const auto z = r.origin().z() + t*r.direction().z();
	const auto y = r.origin().y() + t*r.direction().y();

	//the collision algorithm
	if (z < z0 || z > z1 || y < y0 || y > y1)
		return false;	//there was no collision
	
	//normalise y and z to be used as texture coords
	rec.v = (z-z0)/(z1-z0);
	rec.u = (y-y0)/(y1-y0);

	rec.t = t;

	const auto outward_normal = vec3(1, 0, 0);	//the trival normal vector
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mp;
	rec.p = r.at(t);

	return true;
}

