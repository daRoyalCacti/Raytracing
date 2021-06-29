#pragma once

#include <utility>

#include "camera.h"
#include "hittable.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"

#include "common.h"
#include "bvh.h"
#include "fog.h"

#include "triangle.h"
#include "triangle_mesh.h"

constexpr double aspec1 = 16.0/9.0;

enum class background_color {sky, black};

struct scene_settings {
    scene_settings() {
        important = make_shared<hittable_list>();
    }

    shared_ptr<hittable_list> important;

    bool has_fog = false;
    bool importance = false;
};



struct scene {
    scene_settings settings;

	hittable_list world;
	shared_ptr<camera> cam;
	double aspect_ratio = 0;

    color background;
    shared_ptr<participating_medium> fog;


    [[maybe_unused]] [[nodiscard]] inline shared_ptr<hittable> important_hittables() const {
	    return settings.important;
	}

	scene() = default;

	explicit scene(const double aspec) : aspect_ratio(aspec) {}

    inline void add_important(const shared_ptr<hittable>& obj) {
	    settings.importance = true;
	    settings.important->add(obj);
	}

	inline void set_fog(shared_ptr<participating_medium> f) {
	    settings.has_fog = true;
	    fog =  std::move(f);
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
		cam = make_shared<camera>(lookfrom, lookat, vup, fov, aspect_ratio, aperture, dist_to_focus, time0, time1);
	}	


};




struct [[maybe_unused]] first_scene : public scene {
	explicit first_scene() : scene(aspec1) {
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

struct [[maybe_unused]] big_scene1 : public scene {
    explicit big_scene1() : scene(aspec1) {
		set_background(background_color::sky);


		hittable_list obj;
		
		const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
		//const auto ground_material = make_shared<lambertian>(checker);
        const auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
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




struct [[maybe_unused]] big_scene1_fog : public scene {
    explicit big_scene1_fog() : scene(aspec1) {
        set_background(background_color::sky);


        hittable_list obj;

        const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
        //const auto ground_material = make_shared<lambertian>(checker);
        const auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
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

        set_fog(make_shared<basic_constant_fog>(color(0.8, 0.8, 0.8), 0.8, 0.3) );

        set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));
    }

};


struct [[maybe_unused]] big_scene2 : public scene{
	explicit big_scene2() : scene(1) {
		set_background(background_color::black);

		hittable_list boxes1;
		const auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

		//generating the ground
		const int boxes_per_side = 20;
		const auto w = 100.0;	//width
		for (int i = 0; i < boxes_per_side; i++) 
			for (int j = 0; j < boxes_per_side; j++) {
				const auto x0 = -1000.0 + i*w;
				const auto z0 = -1000.0 + j*w;
				const auto y0 = 0.0;

				const auto x1 = x0 + w;
				const auto z1 = z0 + w;
				const auto y1 = random_double(1, 101);

				boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
			}

		world.add(make_shared<bvh_node>(boxes1, 0, 1));
		
		//main light
		const auto light = make_shared<diffuse_light>(color(7, 7, 7));
		world.add(make_shared<flip_face>(make_shared<xz_rect>(123, 423, 147, 412, 554, light)));

		add_important(make_shared<xz_rect>(123, 423, 147, 412, 554, make_shared<material>()));

		//moving sphere
		const auto center1 = point3(400, 400, 200);
		const auto center2 = center1 + vec3(30, 30, 0);
		const auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
		world.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

		//glass ball
		world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));

		//metal ball
		world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 0.5) ));

		//smoke
		const auto boundary1 = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
		world.add(boundary1);
		world.add(make_shared<constant_medium>(boundary1, 0.2, color(0.2, 0.4, 0.9) ));
		const auto boundary2 = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5) );
		world.add(make_shared<constant_medium>(boundary2, 0.001, color(1,1,1) ));

		//Earth
		const auto emat = make_shared<lambertian>(make_shared<image_texture>("../textures/earthmap.jpg"));
		world.add(make_shared<sphere>(point3(400,200,400), 100, emat));

		//sphere with noise texture
		const auto pertex = make_shared<turbulent_texture>(0.1);
		world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertex) ));

		//box of many spheres
		hittable_list boxes2;
		const auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
		const int ns = 1000;	//number of spheres
		for (int j = 0; j < ns; j++)
			boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white) );

		world.add(make_shared<translate>(make_shared<rotate_y>(make_shared<bvh_node>(boxes2, 0.0, 1.0), 30), vec3(-100, 270, 395) ));

		set_camera(point3(478, 278, -600), point3(278, 278, 0), 40.0, 0.0);
	}
};


struct [[maybe_unused]] two_spheres_scene : public scene {
	two_spheres_scene() : scene(aspec1) {
		set_background(background_color::sky);
		
		const auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

		world.add(make_shared<sphere>(point3(0,-10,0), 10, make_shared<lambertian>(checker) ));
		world.add(make_shared<sphere>(point3(0, 10,0), 10, make_shared<lambertian>(checker) ));
		

		set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));
	}


};



struct [[maybe_unused]] two_perlin_spheres_scene : public scene {
	two_perlin_spheres_scene() : scene(aspec1) {
		set_background(background_color::sky);
		
		const auto pertex1 = make_shared<marble_texture>(4);
		const auto pertex2 = make_shared<turbulent_texture>(5);

		world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertex1) ));
		world.add(make_shared<sphere>(point3(0,    2,0),    2, make_shared<lambertian>(pertex2) ));

		set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));

	}
};



struct [[maybe_unused]] earth_scene : public scene {
	earth_scene() : scene(aspec1) {
		set_background(background_color::black);

		const auto earth_texture = make_shared<image_texture>("../textures/earthmap.jpg");
		const auto earth_surface = make_shared<lambertian>(earth_texture);
		world.add(make_shared<sphere>(point3(0,0,0), 2, earth_surface));

		const auto difflight = make_shared<diffuse_light>(color(4.0, 4.0, 4.0));
		world.add(make_shared<xy_rect>(-5, 5, -3, 3, -6, difflight));


		set_camera(vec3(13.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));
	}
};

struct [[maybe_unused]] earth_atm_scene : public scene {
	earth_atm_scene() : scene(aspec1) {
		set_background(background_color::black);

		const auto earth_texture = make_shared<image_texture>("../textures/earthmap.jpg");
		const auto earth_surface = make_shared<lambertian>(earth_texture);
		world.add(make_shared<sphere>(point3(0,0,0), 2, earth_surface));

		const auto difflight = make_shared<diffuse_light>(color(4.0, 4.0, 4.0));
		world.add(make_shared<xy_rect>(-5, 5, -3, 3, -6, difflight));


		const auto boundary = make_shared<sphere>(point3(0,0,0), 2.5, make_shared<dielectric>(1.5) );
		world.add(make_shared<constant_medium>(boundary, 0.001, color(98.0/255.0, 140.0/255.0, 245.0/255.0)) );
		

		set_camera(vec3(13.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));

	}
};


struct [[maybe_unused]] cornell_box_scene : public scene {
	cornell_box_scene() : scene(1.0) {
		set_background(background_color::black);	//shouldn't matter -- can't see sky

		const auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
		const auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
		const auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
		const auto light_col = make_shared<diffuse_light>(color(15, 15, 15));	//very bright light

		world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));	//left wall
		world.add(make_shared<yz_rect>(0, 555, 0, 555, 0  , red  ));	//right wall

		world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light_col)));	//small light on roof
		add_important(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));

		world.add(make_shared<xz_rect>(0, 555, 0, 555, 0  , white));	//floor
		world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));	//roof

		world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));	//back wall
		//world.add(make_shared<xy_rect>(0, 555, 0, 555, 0  , white));	//front wall
		
		//big box
		shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
		box1 = make_shared<rotate_y>(box1, 15);
		box1 = make_shared<translate>(box1, vec3(265, 0, 295));
		world.add(box1);
		

		//small box
		shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
		box2 = make_shared<rotate_y>(box2, -18);
		box2 = make_shared<translate>(box2, vec3(130,0,65));
		world.add(box2);


		set_camera(vec3(278, 278, -800), vec3(278, 278, 0), 40.0, 0.0); 
	}
};


struct [[maybe_unused]] cornell_box_scene2 : public scene {
    cornell_box_scene2() : scene(1.0) {
        set_background(background_color::black);	//shouldn't matter -- can't see sky

        const auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
        const auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
        const auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
        const auto light_col = make_shared<diffuse_light>(color(15, 15, 15));	//very bright light

        world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));	//left wall
        world.add(make_shared<yz_rect>(0, 555, 0, 555, 0  , red  ));	//right wall

        world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light_col)));	//small light on roof

        world.add(make_shared<xz_rect>(0, 555, 0, 555, 0  , white));	//floor
        world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));	//roof

        world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));	//back wall
        //world.add(make_shared<xy_rect>(0, 555, 0, 555, 0  , white));	//front wall

        //big box
        shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
        box1 = make_shared<rotate_y>(box1, 15);
        box1 = make_shared<translate>(box1, vec3(265, 0, 295));
        world.add(box1);


        //small ball
        const auto glass = make_shared<dielectric>(1.5);
        world.add(make_shared<sphere>(point3(190,90,190), 90, glass));

        //auto lights = make_shared<hittable_list>();
        add_important(make_shared<sphere>(point3(190,90,190), 90, shared_ptr<material>()));
        add_important(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));

        set_camera(vec3(278, 278, -800), vec3(278, 278, 0), 40.0, 0.0);
    }
};

struct [[maybe_unused]] cornell_box_scene2_fog : public scene {
    explicit cornell_box_scene2_fog(const double lambda = 0.01) : scene(1.0) {
        set_background(background_color::black);

        const auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
        const auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
        const auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
        const auto light_col = make_shared<diffuse_light>(color(15, 15, 15));	//very bright light

        world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));	//left wall
        world.add(make_shared<yz_rect>(0, 555, 0, 555, 0  , red  ));	//right wall

        world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light_col)));	//small light on roof

        world.add(make_shared<xz_rect>(0, 555, 0, 555, 0  , white));	//floor
        world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));	//roof

        world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));	//back wall
        //world.add(make_shared<xy_rect>(0, 555, 0, 555, 0  , white));	//front wall

        //big box
        shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
        box1 = make_shared<rotate_y>(box1, 15);
        box1 = make_shared<translate>(box1, vec3(265, 0, 295));
        world.add(box1);

        set_fog(make_shared<basic_constant_fog>(color(0.8, 0.8, 0.8), lambda, 0.3) );    //0.01, 0.007 possible lambda

        //sphere
        const auto glass = make_shared<dielectric>(1.5);
        world.add(make_shared<sphere>(point3(190,90,190), 90, glass));

        //auto lights = make_shared<hittable_list>();
        add_important(make_shared<sphere>(point3(190,90,190), 90, shared_ptr<material>()));
        add_important(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));

        set_camera(vec3(278, 278, -800), vec3(278, 278, 0), 40.0, 0.0);
    }
};



struct [[maybe_unused]] cornell_smoke_box_scene : public scene {
	cornell_smoke_box_scene() : scene(1.0) {
		set_background(background_color::black);	//shouldn't matter -- can't see sky

		const auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
		const auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
		const auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
		const auto light = make_shared<diffuse_light>(color(7, 7, 7));	//very bright light

		world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));	//left wall
		world.add(make_shared<yz_rect>(0, 555, 0, 555, 0  , red  ));	//right wall

		world.add(make_shared<flip_face>(make_shared<xz_rect>(113, 443, 127, 432, 554, light)));	//small light on roof

		world.add(make_shared<xz_rect>(0, 555, 0, 555, 0  , white));	//floor
		world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));	//roof

		world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));	//back wall
		//world.add(make_shared<xy_rect>(0, 555, 0, 555, 0  , white));	//front wall
		

		//big box
		shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
		box1 = make_shared<rotate_y>(box1, 15);
		box1 = make_shared<translate>(box1, vec3(265, 0, 295));
		world.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)) );
		

		//small box
		shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
		box2 = make_shared<rotate_y>(box2, -18);
		box2 = make_shared<translate>(box2, vec3(130,0,65));
		world.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));


		set_camera(vec3(278, 278, -800), vec3(278, 278, 0), 40.0, 0.0); 
	}
};




struct [[maybe_unused]] triangle_scene : public scene {
	triangle_scene() : scene(aspec1) {
		set_background(background_color::sky);	//shouldn't matter -- can't see sky

		world.add(make_shared<triangle>(vec3(-0.5, 0, 0), vec3(0, 1, 10), vec3(0,0,0), 0, 0, 0, 1, 1, 0, make_shared<lambertian>(vec3(0, 1, 0)) ));
		world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100, make_shared<lambertian>(vec3(0,0,1)) ));	


		set_camera(vec3(0, 0, -3), vec3(0, 0, 0), 40.0, 0.0); 
	}
};



struct [[maybe_unused]] door_scene : public scene {
	door_scene() : scene(aspec1) {
		set_background(background_color::sky);	//shouldn't matter -- can't see sky

		world.add(generate_model("../models/door/door.obj"));
		world.add(make_shared<sphere>(vec3(0, -100, -1), 100, make_shared<lambertian>(vec3(0,1,0)) ));	


		set_camera(vec3(-3, 4, -5), vec3(0, 1, 0), 20.0, 0.0); 
	}
};


struct [[maybe_unused]] cup_scene : public scene {
	cup_scene() : scene(aspec1) {
		set_background(background_color::sky);	//shouldn't matter -- can't see sky

		world.add(generate_model("../models/cup/cup.obj"));
		world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100, make_shared<lambertian>(vec3(0,1,0)) ));	


		set_camera(vec3(0, 0, -0.5), vec3(0, 0, 0), 20.0, 0.0); 
	}
};


struct [[maybe_unused]] crate_scene : public scene {
	crate_scene() : scene(aspec1) {
		set_background(background_color::sky);	//shouldn't matter -- can't see sky

		world.add(generate_model("../models/crate/Crate1.obj"));
		world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100, make_shared<lambertian>(vec3(0,1,0)) ));	


		set_camera(vec3(-3, 4, -5), vec3(0, 1, 0), 20.0, 0.0); 
	}
};
