#pragma once

#include "hittable.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "common.h"
#include "bvh.h"


hittable_list first_scene() {	
	hittable_list world;

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
	return world;
}



hittable_list random_scene() {
	hittable_list world;
	
	const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	const auto ground_material = make_shared<lambertian>(checker);
	world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

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
					world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
				} else if (choose_mat < 0.95) {
					//metal
					const auto albedo = color::random(0.5, 1);
					const auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				} else {
					//glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));	
				}
				
			}

		}
	}

	//main balls
	const auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	const auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	const auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	hittable_list objects;
	objects.add(make_shared<bvh_node>(world, 0, 1));
	
	return objects;	
}


camera random_scene_cam(const double aspect_ratio) {
	const point3 lookfrom(13,2,3);
	const point3 lookat(0,0,0);
	const vec3 vup(0,1,0);
	const auto dist_to_focus = 10.0;
	const auto aperture = 0.1;
	const double fov = 20.0;

	return camera(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, 0.0, 0.35);
}



hittable_list two_spheres_scene() {
	hittable_list objects;

	const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	objects.add(make_shared<sphere>(point3(0,-10,0), 10, make_shared<lambertian>(checker) ));
	objects.add(make_shared<sphere>(point3(0, 10,0), 10, make_shared<lambertian>(checker) ));

	return objects;
}

camera two_spheres_scene_cam(const double aspect_ratio) {
	const point3 lookfrom(13, 2, 3);
	const point3 lookat(0, 0, 0);
	const vec3 vup(0, 1, 0);
	const auto dist_to_focus = 10.0;
	const auto aperture = 0.0;
	const double fov = 20.0;

	return camera(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, 0.0, 0.35);
}



hittable_list two_perlin_spheres_scene() {
	hittable_list objects;

	const auto pertex1 = make_shared<marble_texture>(4);
	const auto pertex2 = make_shared<turbulent_texture>(5);

	objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertex1) ));
	objects.add(make_shared<sphere>(point3(0,    2,0),    2, make_shared<lambertian>(pertex2) ));

	return objects;
}

camera two_perlin_spheres_scene_cam(const double aspect_ratio) {
	const point3 lookfrom(13, 2, 3);
	const point3 lookat(0,0,0);
	const vec3 vup(0, 1, 0);
	const auto dist_to_focus = 10.0;
	const auto aperture = 0.0;
	const double fov = 20.0;

	return camera(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, 0.0, 0.35);
}
