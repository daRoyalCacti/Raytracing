#pragma once

#include "common.h"
#include "aabb.h"

struct material;

struct hit_record {
	point3 p;	//point where hit
	vec3 normal;	//normal at hit point
	shared_ptr<material> mat_ptr;
	double t;	//time point was hit
	bool front_face;	//did the hit happen on the front or back of the face
	double u;	//uv coords for textures
	double v;

	inline void set_face_normal(const ray& r, const vec3& outward_normal) { //function to set normal and front_face
		front_face = dot(r.direction(), outward_normal) < 0;	//if the ray direction points against the normal, the ray collided with the front
		normal = front_face ? outward_normal :-outward_normal;	//forcing the normal to point agains the ray direction
									//if the ray collides with the front, the normal is fine
									//if the ray collides with the back, the normal needs to be flipped
	}
};

struct hittable {
	virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const = 0;	//function to tell when a ray hits the object
	virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const = 0;	//function that creates a bounding box around the object

	virtual double pdf_value(const point3& o, const vec3 &v) const {
	    //std::cout << "bad" << std::endl;
	    return 0.0;
	}

	virtual vec3 random(const vec3 &o) const {
	    return vec3(1, 0, 0);
	}
};


struct translate : public hittable {
	shared_ptr<hittable> ptr;
	vec3 offset;

	translate(const shared_ptr<hittable> p, const vec3& displacement) : ptr(p), offset(displacement) {}

	virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const override {
		const ray moved_r(r.origin() - offset, r.direction(), r.time());	//moving object by offset is same as translating axes by -offset
		
		if(!ptr->hit(moved_r, t_min, t_max, rec))	//if ray doesn't hits object in new axes
			return false;

		rec.p += offset;
		rec.set_face_normal(moved_r, rec.normal);

		return true;
	}

	virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		if (!ptr->bounding_box(time0, time1, output_box))	//if there is no bounding box
			return false;					//alse sets output_box

		output_box = aabb(output_box.min() + offset, output_box.max() + offset);

		return true;
	}
};


struct rotate_y : public hittable {
	shared_ptr<hittable> ptr;
	double sin_theta, cos_theta;	//required to carry info from constructor to hit
	bool hasbox;			//required to carry info from constructor to bounding_box
	aabb bbox;

	rotate_y(const shared_ptr<hittable> p, const double angle);

	virtual bool hit(const ray&r, const double t_min, const double t_max, hit_record& rec) const override;

	virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		output_box = bbox;
		return hasbox;
	}
};

rotate_y::rotate_y(const shared_ptr<hittable> p, const double angle) : ptr(p) {
	const auto radians = degrees_to_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);		//sets bbox

	//setting the bounding box
	point3 min( infinity,  infinity,  infinity);	//intilised to rediculous values because used for comparisons
	point3 max(-infinity, -infinity, -infinity);

	//running throung all corners of the box and rotating them
	//required to know which corner is min and which is max
	for (int i = 0; i < 2; i++) 
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++) {
				const auto x = i*bbox.max().x() + (1-i)*bbox.min().x();		//i = 0 gives min,  i = 1 gives max  (this is all possible i)
				const auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
				const auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

				const auto newx =  cos_theta*x + sin_theta*z;	//rotating using Euler angles
				const auto newz = -sin_theta*x + cos_theta*z;

				const vec3 tester(newx, y, newz);	//only used in folling loop

				//checking every component of the rotated vector to see find what is min and max
				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}

	bbox = aabb(min,max);
}

bool rotate_y::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
	auto origin = r.origin();
	auto direction = r.direction();
	
	//rotation of ray using Euler angles
	// - changing basis is the same as rotation
	origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
	origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

	direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
	direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

	const ray rotated_r(origin, direction, r.time());	//where the ray is coming from in the new frame

	if (!ptr->hit(rotated_r, t_min, t_max, rec))	//if the ray doesn't hit in the new frame
		return false;				//also sets rec

	auto p = rec.p;
	auto normal = rec.normal;

	//rotation the position of the collision and the normal vectors using Euler angles
	p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
	p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

	normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
	normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotated_r, normal);

	return true;
}


struct flip_face : public hittable {
    shared_ptr<hittable> ptr;

    flip_face(shared_ptr<hittable> p) : ptr(p) {}

    virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record &rec) const override {
        if (!ptr->hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
        return ptr->bounding_box(time0, time1, output_box);
    }
};
