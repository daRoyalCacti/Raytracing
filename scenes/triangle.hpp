//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_TRIANGLE_HPP
#define RAYTRACER_TRIANGLE_HPP

#include "../scene.hpp"


struct [[maybe_unused]] triangle_scene : public scene {
    triangle_scene() : scene(aspec1) {
        set_background(background_color::sky);	//shouldn't matter -- can't see sky

        world.add(std::make_shared<triangle>(vec3(-0.5, 0, 0), vec3(0, 1, 10),
                                             vec3(0,0,0), 0, 0, 0, 1, 1, 0, std::make_shared<lambertian>(vec3(0, 1, 0)) ));
        world.add(std::make_shared<sphere>(vec3(0, -100.5, -1), 100, std::make_shared<lambertian>(vec3(0,0,1)) ));


        set_camera(vec3(0, 0, -3), vec3(0, 0, 0), 40.0, 0.0);
    }
};


#endif //RAYTRACER_TRIANGLE_HPP
