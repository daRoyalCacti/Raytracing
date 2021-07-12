//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_TWO_SPHERES_HPP
#define RAYTRACER_TWO_SPHERES_HPP



struct [[maybe_unused]] two_spheres_scene : public scene {
    two_spheres_scene() : scene(aspec1) {
        set_background(background_color::sky);

        const auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

        world.add(make_shared<sphere>(point3(0,-10,0), 10, make_shared<lambertian>(checker) ));
        world.add(make_shared<sphere>(point3(0, 10,0), 10, make_shared<lambertian>(checker) ));


        set_camera(vec3(13.0, 2.0, 3.0), vec3(0.0, 0.0, 0.0));
    }


};

#endif //RAYTRACER_TWO_SPHERES_HPP
