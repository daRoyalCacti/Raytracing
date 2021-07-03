//is a tree of bounding boxes
#pragma once

#include <algorithm>
#include "hittable_list.hpp"

/* ====================================================================================================================
This can be made better (i.e. TODO)
1. use SAH https://psgraphics.blogspot.com/2016/03/a-simple-sah-bvh-build.html,
        https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies#TheSurfaceAreaHeuristic (e.q 4.1 helpful and the equation below it)
  - break the volume up into sections that minimise the SA (didn't read fully)
  - pbr better than blogspot
2. need to keep track of if are at a leaf node (an end node)
3. https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies#CompactBVHForTraversal
  - align objects for better caching
4. https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies#Traversal
  - can update t_max on an obj collision to immediately discard a number of boxes
  - do a front to back search through the node
   -- if collision maximum collision time with bounding box is larger than t_max, no need to check children because by definition of a bounding box
      any collision with any other bounding box or any other object must happen at a time after the collision with the bounding box
  - "An efficient way to perform a front-to-back traversal without incurring the expense of intersecting the ray with both child nodes and comparing
    the distances is to use the sign of the ray’s direction vector for the coordinate axis along which primitives were partitioned for the current node:
    if the sign is negative, we should visit the second child before the first child, since the primitives that went into the second child’s subtree
    were on the upper side of the partition point."
   -- this requires the setup in the SAH link
   -- still need to check collision of other nodes because bounding boxes can overlap (more generally the projection of the bounding box
     onto any axis can - and almost certainly will - overlap)
 ===============================================================================================================*/
struct bvh_node : public hittable {
    std::shared_ptr<hittable> left;	//left and right nodes on the tree
    std::shared_ptr<hittable> right;
	aabb box;			//the box for the current node

	bvh_node() = default;
	bvh_node(const hittable_list& list, const double time0, const double time1) : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {}
	bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, size_t start, size_t end, double time0, double time1);

    inline bool hit(const ray& r, double t_min, double t_max, hit_record& rec) override{
        if (!box.hit(r, t_min, t_max)) return false;	//if it didn't hit the large bounding box

        const bool hit_left = left->hit(r, t_min, t_max, rec);	//did the ray hit the left hittable
        const bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);	//did the ray hit the right hittable
        //if the ray hit the left, check to make sure it hit the right before the left
        // - so rec is set correctly
        return hit_left || hit_right;
    }

	inline bool bounding_box(double time0, double time1, aabb& output_box) const override{
        output_box = box;
        return true;
    }
};


inline bool box_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b, const int axis) {
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0,0,box_a) || !b->bounding_box(0,0,box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min()[axis] < box_b.min()[axis];
}

inline bool box_x_compare (const std::shared_ptr<hittable> &a, const std::shared_ptr<hittable> &b) {
	return box_compare(a, b, 0);
}


inline bool box_y_compare (const std::shared_ptr<hittable> &a, const std::shared_ptr<hittable> &b) {
	return box_compare(a, b, 1);
}


inline bool box_z_compare (const std::shared_ptr<hittable> &a, const std::shared_ptr<hittable> &b) {
	return box_compare(a, b, 2);
}

bvh_node::bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, const size_t start, const size_t end, const double time0, const double time1) {
	auto objects = src_objects;	//Create a modifiable array for the source scene objects

	const auto axis = random_int(0,2);
	const auto comparator =   (axis==0) ? box_x_compare	//used to sort boxes into close and far to a given axis
				: (axis==1) ? box_y_compare
					    : box_z_compare;
	
	const size_t object_span = end - start;		//the number of objects the node is conned to

	if (object_span == 1) {		//only 1 object on node
		//put the object in both left and right
		left = right = objects[start];
	} else if (object_span == 2) {	//2 objects on node

		if (comparator(objects[start], objects[start+1])) {	//if the first object is closer to the random axis than the second object
			left = objects[start];
			right = objects[start+1];
		} else {
			left = objects[start+1];
			right = objects[start];
		}

	} else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		const auto mid = start + object_span /2;
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);
	}

	aabb box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right) )
		std::cerr << "No bounding box in bvh_node constructor.\n";
	
	box = surrounding_box(box_left, box_right);
}



