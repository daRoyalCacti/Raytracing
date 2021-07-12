//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_EARTH_HPP
#define RAYTRACER_EARTH_HPP

#include "../scene.hpp"

struct [[maybe_unused]] earth_scene : public scene {
    earth_scene() : scene(aspec1) {
        set_background(background_color::black);

        const auto earth_texture = std::make_shared<image_texture>("../textures/earthmap.jpg");
        const auto earth_surface = std::make_shared<lambertian>(earth_texture);
        world.add(std::make_shared<sphere>(point3(0,0,0), 2, earth_surface));

        const auto difflight = std::make_shared<diffuse_light>(color(4.0, 4.0, 4.0));
        world.add(std::make_shared<xy_rect>(-5, 5, -3, 3, -6, difflight));


        set_camera(vec3(13.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));
    }
};

#endif //RAYTRACER_EARTH_HPP
