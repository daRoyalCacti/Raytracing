#pragma once

#include "hittable.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "common.h"
#include "bvh.h"

enum class background_color {sky, black};



struct scene {
	color background_;
	hittable_list world;
	camera cam_;
	double aspect_ratio;

	inline color background() {
		return background_;
	}

	inline hittable_list objects() {
		return world;
	}

	inline camera cam() {
		return cam_;
	}

	scene(const double aspec) : aspect_ratio(aspec) {}

	inline void set_background(const background_color& col) {
		if (col == background_color::sky) {
			background_ = color(0.70, 0.80, 1.00);
		} else if (col == background_color::black) {
			background_ = color(0.0, 0.0, 0.0);
		} else {
			background_ = color(1.0, 0.0, 1.0);	//terrible color for debugging
		}
	}

	inline void set_camera(const point3 lookfrom, const point3 lookat, const double fov = 20.0, const double aperture = 0.1, const double dist_to_focus = 10.0, 
			const double time0 = 0.0, const double time1 = 0.35, const vec3 vup = vec3(0,1,0)) {
		cam_ = camera(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, time0, time1);
	}	


};




struct first_scene : public scene {
	first_scene(const double aspect) : scene(aspect) {
		set_background(background_color::sky);

		const auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0));
		const auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.3));
		const auto material_left = make_shared<dielectric>(2.5);
		const auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.2);
		const auto material_front = make_shared<dielectric>(2);

		world.add(make_shared<sphere>(point3( 0.0,-100.5,-1.0), 100.0, material_ground));
		world.add(make_shared<sphere>(point3( 0.0,   0.0,-1.0),   0.5, material_center));
		world.add(make_shared<sphere>(point3(-1.0,   0.0,-1.0),   0.5, material_left));
		world.add(make_shared<sphere>(point3( 1.0,   0.0,-1.0),   0.5, material_right));
		//having a sphere with a negative radius allows for the creation of hollow spheres
		world.add(make_shared<sphere>(point3( 0.0,   1.0,-0.75),  0.25, material_front));
		world.add(make_shared<sphere>(point3( 0.0,   1.0,-0.75), -0.25, material_front));

		set_camera(vec3(-2,2,1), vec3(0,0,-1), 20.0, 0.0);
	}
};

struct random_scene : public scene {
	random_scene(const double aspect) : scene(aspect) {
		set_background(background_color::sky);


		hittable_list obj;
		
		const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
		const auto ground_material = make_shared<lambertian>(checker);
		obj.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

		for (int a = -11; a<11; a++) {	//centers of spheres x
			for (int b = -11; b<11; b++) {	//centers of spheres y
				const auto choose_mat = random_double();	//random number to decide what material to use
				const point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

				if ((center - point3(4, 0.2, 0)).length_squared() > 0.9*0.9) {	//not points where main balls go
					shared_ptr<material> sphere_material;

					if (choose_mat < 0.8) {
						//diffuse (has moving spheres)
						const auto albedo = color::random() * color::random();
						sphere_material = make_shared<lambertian>(albedo);	
						const auto center2 = center + vec3(0, random_double(0, 0.5), 0);	//spheres moving downwards at random speeds				
						obj.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
					} else if (choose_mat < 0.95) {
						//metal
						const auto albedo = color::random(0.5, 1);
						const auto fuzz = random_double(0, 0.5);
						sphere_material = make_shared<metal>(albedo, fuzz);
						obj.add(make_shared<sphere>(center, 0.2, sphere_material));
					} else {
						//glass
						sphere_material = make_shared<dielectric>(1.5);
						obj.add(make_shared<sphere>(center, 0.2, sphere_material));	
					}
					
				}

			}
		}

		//main balls
		const auto material1 = make_shared<dielectric>(1.5);
		obj.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

		const auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
		obj.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

		const auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		obj.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

		world.add(make_shared<bvh_node>(obj, 0, 1));


		set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));
	}

};


struct two_spheres_scene : public scene {
	two_spheres_scene(const double aspect) : scene(aspect) {
		set_background(background_color::sky);
		
		const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

		world.add(make_shared<sphere>(point3(0,-10,0), 10, make_shared<lambertian>(checker) ));
		world.add(make_shared<sphere>(point3(0, 10,0), 10, make_shared<lambertian>(checker) ));
		

		set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));
	}


};



struct two_perlin_spheres_scene : public scene {
	two_perlin_spheres_scene(const double aspec) : scene(aspec) {
		set_background(background_color::sky);
		
		const auto pertex1 = make_shared<marble_texture>(4);
		const auto pertex2 = make_shared<turbulent_texture>(5);

		world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertex1) ));
		world.add(make_shared<sphere>(point3(0,    2,0),    2, make_shared<lambertian>(pertex2) ));

		set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));

	}
};



struct earth_scene : public scene {
	earth_scene(const double aspec) : scene(aspec) {
		set_background(background_color::black);

		const auto earth_texture = make_shared<image_texture>("../textures/earthmap.jpg");
		const auto earth_surface = make_shared<lambertian>(earth_texture);
		world.add(make_shared<sphere>(point3(0,0,0), 2, earth_surface));

		set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));
	}
};


