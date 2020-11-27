#pragma once

#include "common.h"

struct material;

struct hit_record {
	point3 p;	//point where hit
	vec3 normal;	//normal at hit point
	shared_ptr<material> mat_ptr;
	double t;	//time point was hit
	bool front_face;	//did the hit happen on the front or back of the face

	inline void set_face_normal(const ray& r, const vec3& outward_normal) { //function to set normal and front_face
		front_face = dot(r.direction(), outward_normal) < 0;	//if the ray direction points against the normal, the ray collided with the front
		normal = front_face ? outward_normal :-outward_normal;	//forcing the normal to point agains the ray direction
									//if the ray collides with the front, the normal is fine
									//if the ray collides with the back, the normal needs to be flipped
	}
};

struct hittable {
	virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const = 0;
};
