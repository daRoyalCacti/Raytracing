//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_EARTH_ATM_HPP
#define RAYTRACER_EARTH_ATM_HPP


struct [[maybe_unused]] earth_atm_scene : public scene {
    earth_atm_scene() : scene(aspec1) {
        set_background(background_color::black);

        const auto earth_texture = std::make_shared<image_texture>("../textures/earthmap.jpg");
        const auto earth_surface = std::make_shared<lambertian>(earth_texture);
        world.add(std::make_shared<sphere>(point3(0,0,0), 2, earth_surface));

        const auto difflight = std::make_shared<diffuse_light>(color(4.0, 4.0, 4.0));
        world.add(std::make_shared<xy_rect>(-5, 5, -3, 3, -6, difflight));
        add_important(std::make_shared<xy_rect>(-5, 5, -3, 3, -6, std::shared_ptr<material>()) );


        const auto boundary = make_shared<sphere>(point3(0,0,0), 3, std::shared_ptr<material>() );
        world.add(std::make_shared<constant_isotropic_medium>(boundary, 0.1, color(98.0/255.0, 140.0/255.0, 245.0/255.0)) );


        set_camera(vec3(13.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));

    }
};

#endif //RAYTRACER_EARTH_ATM_HPP
