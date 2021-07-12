//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_FOGGY_BALLS_HPP
#define RAYTRACER_FOGGY_BALLS_HPP

#include "../scene.hpp"

struct [[maybe_unused]] foggy_balls : public scene {
    explicit foggy_balls(const double d=0.07) : scene(aspec1) {
        set_background(background_color::sky);

        //participating medium should be set first
        const auto fog_bounds = std::make_shared<box>(vec3(-20, 0, -20), vec3(20, 30, 20), nullptr);
        world.add(std::make_shared<constant_Henyey_medium>(fog_bounds, d, 0.9, color(0.8, 0.8, 0.8) ));

        hittable_list obj;

        const auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
        world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

        for (int a = -20; a<30; a++) {	//centers of spheres x
            for (int b = -11; b<11; b++) {	//centers of spheres z
                const auto choose_mat = random_double();	//random number to decide what material to use
                const point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

                //if ((center - point3(4, 0.2, 0)).length_squared() > 0.9*0.9) {	//not points where main balls go
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    //diffuse (has moving spheres)
                    const auto albedo = random_vec3() * random_vec3();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    const auto center2 = center + vec3(0, random_double(0, 0.5), 0);	//spheres moving downwards at random speeds
                    obj.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    //metal
                    const auto albedo = random_vec3(0.5, 1);
                    const auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    obj.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    //glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    obj.add(make_shared<sphere>(center, 0.2, sphere_material));
                }

                //}

            }
        }

        //main balls
        const auto material1 = std::make_shared<dielectric>(1.5);
        obj.add(std::make_shared<sphere>(point3(0, 1, 3), 1.0, material1));

        //const auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
        //obj.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

        const auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
        obj.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

        world.add(std::make_shared<bvh>(obj, 0, 1));


        set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));

    }

};

#endif //RAYTRACER_FOGGY_BALLS_HPP
