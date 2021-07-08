//is a tree of bounding boxes
#pragma once

#include <algorithm>

/* ====================================================================================================================
This can be made better (i.e. TODO)
1. use SAH https://psgraphics.blogspot.com/2016/03/a-simple-sah-bvh-build.html,
        https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies#TheSurfaceAreaHeuristic (e.q 4.1 helpful and the equation below it)
  - break the volume up into sections that minimise the SA (didn't read fully)
  - pbr better than blogspot
  - a partition of the primitives along the chosen axis that gives a minimal SAH cost
2. need to keep track of if are at a leaf node (an end node)
3. https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies#CompactBVHForTraversal
  - align objects for better caching
4. https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies#Traversal
  - can update t_max on an obj collision to immediately discard a number of boxes
  - do a front to back search through the node
   -- if minimum collision time with bounding box is larger than t_max, no need to check children because by definition of a bounding box
      any collision with any other bounding box or any other object must happen at a time after the collision with the bounding box
  - "An efficient way to perform a front-to-back traversal without incurring the expense of intersecting the ray with both child nodes and comparing
    the distances is to use the sign of the ray’s direction vector for the coordinate axis along which primitives were partitioned for the current node:
    if the sign is negative, we should visit the second child before the first child, since the primitives that went into the second child’s subtree
    were on the upper side of the partition point."
   -- this requires the setup in the SAH link
   -- still need to check collision of other nodes because bounding boxes can overlap (more generally the projection of the bounding box
     onto any axis can - and almost certainly will - overlap)
 ===============================================================================================================*/
struct bvh_node {
    std::shared_ptr<hittable> left_obj;	//left and right nodes on the tree
    std::shared_ptr<hittable> right_obj;

    std::shared_ptr<bvh_node> left_node;	//left and right nodes on the tree
    std::shared_ptr<bvh_node> right_node;

    bool is_leaf = false;   //says whether the obj vars are set or the node vars

	aabb box{};			//the box for the current node
	unsigned split_axis{}; //the axis that the objects were split along
	                    // - second box is always along the positive axis of the first box

	bvh_node() = default;
	bvh_node(const hittable_list& list, double time0, double time1) : bvh_node(list.objects, time0, time1) {}
    bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, double time0, double time1);

    /*inline bool hit_time(const ray& r, double t_min, double t_max, hit_record& rec) override{
        if (!box.hit(r, t_min, t_max)) return false;	//if it didn't hit the large bounding box

        "An efficient way to perform a front-to-back traversal without incurring the expense of intersecting the ray with both child nodes and comparing
        the distances is to use the sign of the ray’s direction vector for the coordinate axis along which primitives were partitioned for the current node:
        if the sign is negative, we should visit the second child before the first child, since the primitives that went into the second child’s subtree
        were on the upper side of the partition point."
        if (r.dir[split_axis] > 0) {
            const bool hit_left = left->hit_time(r, t_min, t_max, rec);	//did the ray hit the left hittable
            const bool hit_right = right->hit_time(r, t_min, hit_left ? rec.t : t_max, rec);	//did the ray hit the right hittable
            //if the ray hit the left, check to make sure it hit the right before the left
            // - so rec is set correctly

            if (hit_right) {
                right->hit_info(r, t_min, hit_left ? rec.t : t_max, rec);
            } else {
                if (hit_left) {
                    left->hit_info(r, t_min, t_max, rec);
                }
            }
            return hit_left || hit_right;
        } else {
            const bool hit_right = right->hit_time(r, t_min, t_max, rec);	//did the ray hit the left hittable
            const bool hit_left = left->hit_time(r, t_min, hit_right ? rec.t : t_max, rec);	//did the ray hit the right hittable
            //if the ray hit the left, check to make sure it hit the right before the left
            // - so rec is set correctly

            if (hit_left) {
                left->hit_info(r, t_min, hit_right ? rec.t : t_max, rec);
            } else {
                if (hit_right) {
                    right->hit_info(r, t_min, t_max, rec);
                }
            }
            return hit_left || hit_right;
        }

    }

    inline void hit_info(const ray& r, double t_min, double t_max, hit_record& rec) override {
        //no need here
    }

	inline bool bounding_box(double time0, double time1, aabb& output_box) const override{
        output_box = box;
        return true;
    }*/
};


inline bool box_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b, const int axis) {
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0,0,box_a) || !b->bounding_box(0,0,box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.mid_point()[axis] < box_b.mid_point()[axis];
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

struct bucket_info {
    //unsigned count = 0;
    aabb bounds = aabb(vec3(0,0,0), vec3(0,0,0));
    std::vector<std::shared_ptr<hittable>> objs;

    [[nodiscard]] inline size_t count() const {
        return objs.size();
    }

    inline void add_obj(const std::shared_ptr<hittable> &object) {
        objs.push_back(object);
    }

    inline void set_bounds(const aabb& b) {
        if ( (bounds.min() == vec3(0,0,0) )  && (bounds.max() == vec3(0,0,0) ) ) {
            bounds = b;
        } else {
            bounds = surrounding_box(bounds, b);
        }
    }
};

struct cost_info {
    std::vector<std::shared_ptr<hittable>> objs0, objs1;
    aabb b0, b1;
    double cost;

    template <typename T>
    void add_hittables_0(const T& objs) {
        for (const auto& o : objs) {
            objs0.push_back(o);
        }
    }

    template <typename T>
    void add_hittables_1(const T& objs) {
        for (const auto& o : objs) {
            objs1.push_back(o);
        }
    }
};



bvh_node::bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, const double time0, const double time1) {
    const auto num_objs = src_objects.size();


    //if <=4, use basic splitting
    // else use SAH
    if (num_objs <= 4) {    //used to be 4
        auto objs_0 = src_objects;

        std::vector<aabb> boxes_0;
        boxes_0.resize(num_objs);

        for (size_t i = 0; i < num_objs; i++) {
            objs_0[i]->bounding_box(0, 0, boxes_0[i]);
        }

        box = boxes_0[0];
        for (size_t i = 1; i < num_objs; i++) {
            box = surrounding_box(box, boxes_0[i]);
        }

        //splitting the boxes along the longest side
        split_axis = box.longest_axis();

        const auto comparator = (split_axis == 0)
                                ? box_x_compare    //used to sort boxes into close and far to a given axis
                                : (split_axis == 1) ? box_y_compare
                                             : box_z_compare;
        if (num_objs == 1) {		//only 1 object on node
            //put the object in both left and right
            left_obj = right_obj = objs_0[0];
            is_leaf = true;
        } else if (num_objs == 2) {	//2 objects on node

            if (comparator(objs_0[0], objs_0[1])) {	//if the first object is closer to the random axis than the second object
                left_obj = objs_0[0];
                right_obj = objs_0[1];
            } else {
                left_obj = objs_0[1];
                right_obj = objs_0[0];
            }
            is_leaf = true;

        } else {
            std::sort(objs_0.begin(),  objs_0.end(), comparator);

            const auto mid = static_cast<size_t>(num_objs /2);
            std::vector<std::shared_ptr<hittable>> objs_left, objs_right;
            objs_left.resize(mid);
            objs_right.resize(num_objs-mid);

            for (size_t i = 0; i < mid; i++) {
                objs_left[i] = objs_0[i];
            }
            for (size_t i = mid; i < num_objs; i++) {
                objs_right[i-mid] = objs_0[i];
            }

            is_leaf = false;
            left_node = make_shared<bvh_node>(objs_left, time0, time1);
            right_node = make_shared<bvh_node>(objs_right, time0, time1);
        }

     } else {

         int best_dim = -1;
         cost_info c;   //best cost
         double min_cost = infinity;


         for (unsigned dim = 0; dim < 3; ++dim) {
             const auto comparator = (dim == 0)
                                     ? box_x_compare    //used to sort boxes into close and far to a given axis
                                     : (dim == 1) ? box_y_compare
                                                  : box_z_compare;
             constexpr size_t num_buckets = 12;   //find a good number (TODO)
             // - also allow to just have every obj in its own buffer (TODO)
             auto objs_0 = src_objects;
             std::sort(objs_0.begin(), objs_0.end(), comparator);
             std::vector<aabb> boxes_0;
             boxes_0.resize(num_objs);

             for (size_t i = 0; i < num_objs; i++) {
                 objs_0[i]->bounding_box(0, 0, boxes_0[i]);
             }

             const double min = boxes_0[0].mid_point()[dim];
             const double max = boxes_0[num_objs - 1].mid_point()[dim];
             const double range = max - min;

             //setting buckets
             // - finding the number of objects in each bucket
             // - finding the bounds for the objects in the bucket
             std::array<bucket_info, num_buckets> buckets;
             for (size_t i = 0; i < num_objs; i++) {
                 const double mid_point = boxes_0[i].mid_point()[dim];
                 if (range == 0) {
                     continue;//2 objects with same bounding box is not supported
                     //std::cout << "need to pick another axis\n";
                 }
                 const double normalized_mid_point =
                         (mid_point - min) / range * num_buckets;    //midpoint mapped to [0,num_buckets]
 #ifndef NDEBUG
                 auto b = static_cast<size_t>(normalized_mid_point);
                 if (b == num_buckets) { b = num_buckets - 1; }    //happens at the endpoints
                 if (b > num_buckets) {  //some error checking
                     std::cerr << "trying to fill bucket out of range\n";
                 }
 #else
                 const auto b = std::min(static_cast<size_t>(normalized_mid_point),
                                         static_cast<size_t>(num_buckets - 1));//gives an index in the buckets array
                                                                     //b = num_buckets - 1 happens at the endpoints
 #endif
                 buckets[b].add_obj(objs_0[i]);
                 buckets[b].set_bounds(boxes_0[i]);
             }

             //computing the cost of each reasonable combination of buckets (i.e. all on the left or right of some point)
             std::array<cost_info, num_buckets - 1> cost;
             for (size_t i = 0; i < num_buckets - 1; i++) {
                 cost[i].b0 = buckets[0].bounds;     //the bounds for each combination of buckets
                 cost[i].b1 = buckets[i + 1].bounds;
                 unsigned counter0 = buckets[0].count(), counter1 = buckets[i + 1].count();

                 cost[i].add_hittables_0(buckets[0].objs);
                 cost[i].add_hittables_1(buckets[i + 1].objs);

                 for (size_t j = 1; j <= i; j++) {
                     cost[i].b0 = surrounding_box(cost[i].b0, buckets[j].bounds);
                     cost[i].add_hittables_0(buckets[j].objs);
                     counter0 += buckets[j].count();
                 }
                 for (size_t j = i + 2; j < num_buckets; j++) {
                     cost[i].b1 = surrounding_box(cost[i].b1, buckets[j].bounds);
                     cost[i].add_hittables_1(buckets[j].objs);
                     counter1 += buckets[j].count();
                 }

                 //eq 4.1 of pbr, p_a = area(a) / total_area, all objects take equal time to intersect (can remove this assumption; TODO)
                 // - https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies
                 cost[i].cost = 0.125 + (counter0 * cost[i].b0.surface_area() + counter1 * cost[i].b1.surface_area()) /
                                        surrounding_box(cost[i].b0, cost[i].b1).surface_area();
             }

             //finding the minimum cost
             double minCost = infinity;
             int minCost_id = -1;
             for (size_t i = 0; i < cost.size(); i++) {
                 if (cost[i].objs0.empty() || cost[i].objs1.empty()) {//is possible not the have any hittalbes in bucket
                     continue;
                 }
                 if (cost[i].cost < minCost) {
                     minCost = cost[i].cost;
                     minCost_id = i;
                 }

             }

             if (minCost_id == -1) {
                 continue;
                 //std::cerr << "Could not find split that gave non-zero hittables\n";
             }

             //finding the best axis
             if (cost[minCost_id].cost < min_cost) {
                 c = cost[minCost_id];
                 best_dim = static_cast<int>(dim);
             }
         }
 #ifndef NDEBUG
         if (best_dim == -1) {
             std::cerr << "could not find dimension to split axis objects\n";
         }
 #endif

         box = surrounding_box(c.b0, c.b1);
         split_axis = best_dim;

#ifndef NDEBUG
        if (c.objs0.empty()) {
                 std::cerr << "creation of bvh gave 0 objects in left hittable\n";
                 std::cerr << "\tthe right hittable got " << c.objs1.size() << " hittables\n";
             }
#endif


#ifndef NDEBUG
        if (c.objs1.empty()) {
                 std::cerr << "creation of bvh gave 0 objects in right hittable\n";
                 std::cerr << "\tthe left hittable got " << c.objs0.size() << " hittables\n";
             }
#endif


        is_leaf = false;
         //setting the left and right objects
         left_node = std::make_shared<bvh_node>(c.objs0, time0, time1);
         right_node = std::make_shared<bvh_node>(c.objs1, time0, time1);


     }

}



/*
bvh_node::bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, const double time0, const double time1) {
    const auto num_objs = src_objects.size();
    unsigned dim = 0;
    split_axis = 0; //for testing
    auto objs_0 = src_objects;
    std::sort(objs_0.begin(), objs_0.end(), box_x_compare);
    std::vector<aabb> boxes_0;
    boxes_0.resize(num_objs);

    std::cout << "A\n";
    for (size_t i = 0; i < num_objs; i++) {
        objs_0[i]->bounding_box(0,0,boxes_0[i]);
    }

    const double min = boxes_0[0].minimum[dim];
    const double max = boxes_0[num_objs-1].maximum[dim];
    const double range = max - min;

    std::cout << "B\n";

    std::cout << "C\n";
    //computing the cost of each reasonable combination of buckets (i.e. all on the left or right of some point)
    std::vector<cost_info> cost;
    cost.resize(num_objs - 1);
    for (size_t i = 0; i < num_objs - 1; i++) {
        cost[i].b0 = boxes_0[0];     //the bounds for each combination of buckets
        cost[i].b1 = boxes_0[i+1];
        unsigned counter0 = 1, counter1 = 1;

        cost[i].objs0.push_back(objs_0[0]);
        cost[i].objs1.push_back(objs_0[i+1]);

        for (size_t j = 1; j <=i; j++) {
            cost[i].b0 = surrounding_box(cost[i].b0, boxes_0[j]);
            cost[i].objs0.push_back(objs_0[j]);
            counter0 += 1;
        }
        for (size_t j = i+2; j < num_objs; j++) {
            cost[i].b1 = surrounding_box(cost[i].b1, boxes_0[j]);
            cost[i].objs1.push_back(objs_0[j]);
            counter1 += 1;
        }

        //eq 4.1 of pbr, p_a = area(a) / total_area, all objects take equal time to intersect (can remove this assumption; TODO)
        // - https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies
        cost[i].cost = (counter0 * cost[i].b0.surface_area() + counter1 * cost[i].b1.surface_area()) / surrounding_box(cost[i].b0, cost[i].b1).surface_area();
    }

    std::cout << "D\n";
    //finding the minimum cost
    double minCost = infinity;
    int minCost_id = -1;
    for (size_t i = 0; i < cost.size(); i++) {
        //if (cost[i].objs0.empty() || cost[i].objs1.empty()) { //is possible not the have any hittalbes in bucket
        std::cout << cost[i].objs0.size() << " " << cost[i].objs1.size() << "\n";
        if ( (cost[i].objs0.size() == 0) || (cost[i].objs1.size() == 0 ) ) {
            continue;
        }
        if (cost[i].cost < minCost) {
            minCost = cost[i].cost;
            minCost_id = i;
        }

    }

    if (minCost_id == -1) {
        std::cerr << "Could not find split that gave non-zero hittables\n";
    }

    std::cout << "E\n";
    const auto c = &cost[minCost_id];


    //should then pick the dimension here

    //setting the left and right objects
    if (c->objs0.size() > 1) {
        left = std::make_shared<bvh_node>(c->objs0, time0, time1);
    } else {
        if (c->objs0.size() ==0) {
            std::cerr << "creation of bvh gave 0 objects in left hittable\n";
            std::cerr << "\tthe right hittable got " << c->objs1.size() << " hittables\n";
        }
        left = c->objs0[0];
    }

    if (c->objs1.size() > 1) {
        right = std::make_shared<bvh_node>(c->objs1, time0, time1);
    } else {
        if (c->objs1.size() ==0) {
            std::cerr << "creation of bvh gave 0 objects in right hittable\n";
            std::cerr << "\tthe left hittable got " << c->objs0.size() << " hittables\n";
        }
        right = c->objs1[0];
    }
    std::cout << "F\n";
}
 */

