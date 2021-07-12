//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_CORNELL_BOX_HPP
#define RAYTRACER_CORNELL_BOX_HPP


struct [[maybe_unused]] cornell_box_scene : public scene {
    explicit cornell_box_scene(const double d = 0.0005) : scene(1.0) {
        set_background(background_color::black);	//shouldn't matter -- can't see sky

        //participating medium should be set first
        const auto fog_bounds = std::make_shared<box>(vec3(0, 0, 0), vec3(555, 555, 555), nullptr);
        world.add(std::make_shared<constant_isotropic_medium>(fog_bounds, d, color(0.8, 0.8, 0.8) ));

        const auto red = std::make_shared<lambertian>(color(0.65, 0.05, 0.05));
        const auto white = std::make_shared<lambertian>(color(0.73, 0.73, 0.73));
        const auto green = std::make_shared<lambertian>(color(0.12, 0.45, 0.15));
        const auto light_col = std::make_shared<diffuse_light>(color(15, 15, 15));	//very bright light

        world.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green));	//left wall
        world.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0  , red  ));	//right wall

        world.add(std::make_shared<flip_face>(std::make_shared<xz_rect>(213, 343, 227, 332, 554, light_col)));	//small light on roof
        add_important(std::make_shared<xz_rect>(213, 343, 227, 332, 554, std::shared_ptr<material>()));

        world.add(make_shared<xz_rect>(0, 555, 0, 555, 0  , white));	//floor
        world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));	//roof

        world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));	//back wall
        //world.add(make_shared<xy_rect>(0, 555, 0, 555, 0  , white));	//front wall

        //big box
        std::shared_ptr<hittable> box1 = std::make_shared<box>(point3(0,0,0), point3(165,330,165), white);
        box1 = std::make_shared<rotate_y>(box1, 15);
        box1 = std::make_shared<translate>(box1, vec3(265, 0, 295));
        world.add(box1);


        //small box
        std::shared_ptr<hittable> box2 = std::make_shared<box>(point3(0,0,0), point3(165,165,165), white);
        box2 = std::make_shared<rotate_y>(box2, -18);
        box2 = std::make_shared<translate>(box2, vec3(130,0,65));
        world.add(box2);



        set_camera(vec3(278, 278, -800), vec3(278, 278, 0), 40.0, 0.0);
    }
};

#endif //RAYTRACER_CORNELL_BOX_HPP
