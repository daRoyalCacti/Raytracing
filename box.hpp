#pragma once

#include "hittable_list.hpp"

struct box : public hittable {
	const point3 box_min;		//min and max define the corners of the box in the standard way
	const point3 box_max;
	hittable_list sides;

	box() = delete;
	box(const point3& p0, const point3& p1, const std::shared_ptr<material> &ptr);

    inline bool hit_time(const ray& r, const double t_min, const double t_max, hit_record& rec) override {
		return sides.hit_time(r, t_min, t_max, rec);
	}

    inline void hit_info(const ray& r, const double t_min, const double t_max, hit_record& rec) override {
        sides.hit_info(r, t_min, t_max, rec);
    }

    inline bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		output_box = aabb(box_min, box_max);    //the trivial bounding box
		return true;
	}
};

box::box(const point3& p0, const point3& p1, const std::shared_ptr<material> &ptr) : box_min(p0), box_max(p1) {
    sides.reserve(6);   //not really needed -- might minorly improve computations time
	sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

	sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
	sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
	
	sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
	sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}
