#pragma once

#include "camera.hpp"
#include "sphere.hpp"
#include "moving_sphere.hpp"
#include "aarect.hpp"
#include "box.hpp"
#include "constant_medium.hpp"

#include "helpful.hpp"
#include "bvh_node.hpp"

#include "triangle.hpp"
#include "triangle_mesh.hpp"

constexpr double aspec1 = 16.0/9.0;

enum class background_color {sky, black};

struct scene_settings {
    scene_settings() {
        important = std::make_shared<hittable_list>();
    }

    std::shared_ptr<hittable_list> important;

    bool importance = false;
};



struct scene {
    scene_settings settings;

	hittable_list world;
    std::shared_ptr<camera> cam;
	double aspect_ratio = 0;

    color background;

	scene() = default;

	explicit scene(const double aspec) : aspect_ratio(aspec) {}

    inline void add_important(const std::shared_ptr<hittable>& obj) {
	    settings.importance = true;
	    settings.important->add(obj);
	}


	inline void set_background(const background_color& col) {
		if (col == background_color::sky) {
			background = color(0.70, 0.80, 1.00);
		} else if (col == background_color::black) {
			background = color(0.0, 0.0, 0.0);
		} else {
			background = color(1.0, 0.0, 1.0);	//terrible color for debugging
		}
	}

	inline void set_camera(const point3 lookfrom, const point3 lookat, const double fov = 20.0, const double aperture = 0.1, const double dist_to_focus = 10.0, 
			const double time0 = 0.0, const double time1 = 0.35, const vec3 vup = vec3(0,1,0)) {
		cam = std::make_shared<camera>(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, time0, time1);
	}	


};





