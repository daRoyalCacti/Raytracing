#pragma once

#include "hittable.h"
#include "aabb.h"

#include <memory>
#include <vector>


using std::shared_ptr;
using std::make_shared;

struct hittable_list : public hittable {
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() = default;
	explicit hittable_list(const shared_ptr<hittable> &object) {add(object);}

    [[maybe_unused]] void clear() {objects.clear();}
	void add(const shared_ptr<hittable>& object) {objects.push_back(object);}

	bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	bool bounding_box(double time0, double time1, aabb& output_box) const override;

    [[nodiscard]] double pdf_value(const point3& o, const vec3& v) const override;
    [[nodiscard]] vec3 random(const point3& o) const override;
};

bool hittable_list::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;
	
	//testing to see if ray hits anyobject between the given times
	for (const auto& object : objects) {	//iterating through all objects that could be hit
		if (object->hit(r, t_min, closest_so_far, temp_rec)) {//checking to see if the ray hit the object
			hit_anything = true;
			closest_so_far = temp_rec.t;	//keep lowering max time to find closest object
			rec = temp_rec;
		}
	}

	return hit_anything;
}

bool hittable_list::bounding_box(const double time0, const double time1, aabb& output_box) const {
	if (objects.empty()) return false;	//no objects to create bounding boxes for

	aabb temp_box;
	bool first_box = true;

	for (const auto& object : objects) {
		if (!object->bounding_box(time0, time1, temp_box)) return false;	//if bounding box returns false,  this function returns false
											//this also makes temp_box the bounding box for object
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box); 	//creates a bounding box around the previous large bounding box and 
												// the bounding box for the current object
												//If there is no previous large bounding box, output box is just the bounding box
												// for the object
		first_box = false;
	}
	
	return true;
}

double hittable_list::pdf_value(const point3& o, const vec3& v) const {
    const double weight = 1.0/objects.size();
    double sum = 0.0;

    for (const auto& object : objects)
        sum += weight * object->pdf_value(o, v);

    return sum;
}

vec3 hittable_list::random(const point3& o) const {
    const auto int_size = static_cast<int>(objects.size());
    return objects[random_int(0, int_size-1)]->random(o);
}
