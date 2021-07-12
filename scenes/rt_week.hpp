//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_RT_WEEK_HPP
#define RAYTRACER_RT_WEEK_HPP

#include "../scene.hpp"


struct [[maybe_unused]] rt_week : public scene{
    explicit rt_week() : scene(1) {
        set_background(background_color::black);

        hittable_list boxes1;
        const auto ground = std::make_shared<lambertian>(color(0.48, 0.83, 0.53));

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

        world.add(std::make_shared<bvh>(boxes1, 0, 1));

        //main light
        const auto light = std::make_shared<diffuse_light>(color(7, 7, 7));
        world.add(make_shared<flip_face>(make_shared<xz_rect>(123, 423, 147, 412, 554, light)));

        add_important(make_shared<xz_rect>(123, 423, 147, 412, 554, std::make_shared<material>()));

        //moving sphere
        const auto center1 = point3(400, 400, 200);
        const auto center2 = center1 + vec3(30, 30, 0);
        const auto moving_sphere_material = std::make_shared<lambertian>(color(0.7, 0.3, 0.1));
        world.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

        //glass ball
        world.add(make_shared<sphere>(point3(260, 150, 45), 50, std::make_shared<dielectric>(1.5)));

        //metal ball
        world.add(make_shared<sphere>(point3(0, 150, 145), 50, std::make_shared<metal>(color(0.8, 0.8, 0.9), 0.5) ));

        //smoke
        const auto boundary1 = make_shared<sphere>(point3(360, 150, 145), 70, std::make_shared<dielectric>(1.5));
        world.add(boundary1);
        world.add(make_shared<constant_isotropic_medium>(boundary1, 0.2, color(0.2, 0.4, 0.9) ));
        const auto boundary2 = make_shared<sphere>(point3(0,0,0), 5000, std::make_shared<dielectric>(1.5) );
        world.add(make_shared<constant_isotropic_medium>(boundary2, 0.0001, color(1,1,1) ));

        //Earth
        const auto emat = make_shared<lambertian>(std::make_shared<image_texture>("../textures/earthmap.jpg"));
        world.add(make_shared<sphere>(point3(400,200,400), 100, emat));

        //sphere with noise texture
        const auto pertex = std::make_shared<turbulent_texture>(0.1);
        world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertex) ));

        //box of many spheres
        hittable_list boxes2;
        const auto white = std::make_shared<lambertian>(color(0.73, 0.73, 0.73));
        const auto met = std::make_shared<metal>(vec3(0.8, 0.8, 0.8), 1);
        const size_t ns = 1000;	//number of spheres
        for (size_t j = 0; j < ns; j++) {
            const auto ra = random_double();
            if (ra > 0.4) {
                boxes2.add(make_shared<sphere>(random_vec3(0, 165), 10, white));
            } else {
                boxes2.add(make_shared<sphere>(random_vec3(0, 165), 10, met));
            }
        }

        world.add(make_shared<translate>(make_shared<rotate_y>(std::make_shared<bvh>(boxes2, 0.0, 1.0), 30), vec3(-100, 270, 395) ));

        set_camera(point3(478, 278, -600), point3(278, 278, 0), 40.0, 0.0);
    }
};

#endif //RAYTRACER_RT_WEEK_HPP
