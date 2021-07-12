//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_TWO_PERLIN_SPHERES_HPP
#define RAYTRACER_TWO_PERLIN_SPHERES_HPP

#include "../scene.hpp"

struct [[maybe_unused]] two_perlin_spheres_scene : public scene {
    two_perlin_spheres_scene() : scene(aspec1) {
        set_background(background_color::sky);

        const auto pertex1 = std::make_shared<marble_texture>(4);
        const auto pertex2 = std::make_shared<turbulent_texture>(5);

        world.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, std::make_shared<lambertian>(pertex1) ));
        world.add(std::make_shared<sphere>(point3(0,    2,0),    2, std::make_shared<lambertian>(pertex2) ));

        set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));

    }
};

#endif //RAYTRACER_TWO_PERLIN_SPHERES_HPP
