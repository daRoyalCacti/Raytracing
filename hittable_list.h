#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

struct hittable_list : public hittable {
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) {add(object);}

	void clear() {objects.clear();}
	void add(shared_ptr<hittable> object) {objects.push_back(object);}

	virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const override;
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
