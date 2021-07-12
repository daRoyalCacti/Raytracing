//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_FIRST_SCENE_HPP
#define RAYTRACER_FIRST_SCENE_HPP

#include "../scene.hpp"


struct [[maybe_unused]] first_scene : public scene {
    explicit first_scene() : scene(aspec1) {
        set_background(background_color::sky);

        const auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0));
        const auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.3));
        const auto material_left = std::make_shared<dielectric>(2.5);
        const auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.2);
        const auto material_front = std::make_shared<dielectric>(2);

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

#endif //RAYTRACER_FIRST_SCENE_HPP
