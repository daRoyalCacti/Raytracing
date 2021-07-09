//
// Created by jacob on 8/7/21.
//

#ifndef RAYTRACER_BVH_HPP
#define RAYTRACER_BVH_HPP

#include "hittable_list.hpp"
#include "bvh_node.hpp"

//could probably be made to be 32_bytes
struct bvh_info {
    aabb box{};
    union { //one is for leaf nodes and one is for interior nodes
        unsigned primitives_offset; //for leaf nodes
        unsigned second_child_offset;   //first child is immediately next to parent
                                        //https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/BVH%20linearization.svg
    };
    unsigned axis = 4;  //should error out if called and not set
    bool is_leaf{};

};

struct bvh : public hittable {
    std::vector<std::shared_ptr<hittable>> objs;  //filled in the order they appear when constructing the tree
    std::vector<bvh_info> node_info;

    bvh(const hittable_list& list, double time0, double time1);

    bool hit_time(const ray& r, double t_min, double t_max, hit_record& rec) override {
        bool did_hit = false;
        size_t closest_hit;
        size_t current_index = 0;
        constexpr size_t nodes_to_visit_size = 64;
        std::array<unsigned, nodes_to_visit_size> nodes_to_visit;
        unsigned visiting_index = 0;
        rec.t = t_max;
        while (true) {
            const auto curr_node = &node_info[current_index];
            //TODO : update aabb.hit to give the hit time on the other side of the box so t_max can be updated
            if (curr_node->box.hit(r, t_min, rec.t)) {  //if hit the bounding box
                if (curr_node->is_leaf) {   //if at a leaf node
                    if (objs[curr_node->primitives_offset]->hit_time(r, t_min, rec.t, rec)) { //if hit left object
                        did_hit = true;
                        closest_hit = curr_node->primitives_offset;
                        //checking hitting with right object
                        if (objs[curr_node->primitives_offset + 1]->hit_time(r, t_min, rec.t, rec)) {
                            closest_hit = curr_node->primitives_offset+1;
                        }
                    } else {    //else didn't hit left object
                        //checking hitting with right object
                        if (objs[curr_node->primitives_offset + 1]->hit_time(r, t_min, rec.t, rec)) {
                            did_hit = true;
                            closest_hit = curr_node->primitives_offset+1;
                        }
                    }

                    if (visiting_index == 0) break;
                    current_index = nodes_to_visit[--visiting_index];
                } else {    //else not at a leaf node
                    //picking what direction to travel down first
                    if (r.dir[curr_node->axis] < 0) {    //right to left
                        nodes_to_visit[visiting_index++] = current_index + 1;
                        current_index = curr_node->second_child_offset;
                    } else {    //else check collisions left to right
                        nodes_to_visit[visiting_index++] = curr_node->second_child_offset;
                        current_index++;
                    }
#ifndef NDEBUG
                    //error checking
                    if (visiting_index >= nodes_to_visit_size) {
                        std::cerr << "trying to access nodes_to_visit out of range\n";
                    }
#endif
                }
            } else {    //else missed the bounding box
                if (visiting_index == 0) break;
                current_index = nodes_to_visit[--visiting_index];
            }

        }   //end while

        if (did_hit) {
            //update rec with the closest hit
            objs[closest_hit]->hit_info(r, t_min, rec.t, rec);
        }

        return did_hit;
    }
    inline void hit_info(const ray& r, double t_min, double t_max, hit_record& rec) override {
        //not needed
    }

    inline bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = node_info[0].box;  //node_info 0 is the source node
        return true;
    }
private:
    void flatten_tree(const std::shared_ptr<bvh_node>& node);
};

bvh::bvh(const hittable_list& list, double time0, double time1) {
    //first construct the bvh_tree through bvh_nodes
    const auto source_node = std::make_shared<bvh_node>(list, time0, time1);
    flatten_tree(source_node);
}

void bvh::flatten_tree(const std::shared_ptr<bvh_node>& node) {

    node_info.emplace_back();
    const auto curr_node = node_info.end() - 1;
    curr_node->box = node->box;
    curr_node->axis = node->split_axis;
    curr_node->is_leaf = node->is_leaf;

    if (node->is_leaf) {
        objs.push_back(node->left_obj);
        objs.push_back(node->right_obj);
        curr_node->primitives_offset = objs.size() - 2;
    } else {
        const auto up_to = node_info.size() - 1;
        flatten_tree(node->left_node);
        node_info[up_to].second_child_offset = node_info.size();
        flatten_tree(node->right_node);
    }
}

#endif //RAYTRACER_BVH_HPP
