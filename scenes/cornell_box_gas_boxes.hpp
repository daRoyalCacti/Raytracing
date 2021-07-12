//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_CORNELL_BOX_GAS_BOXES_HPP
#define RAYTRACER_CORNELL_BOX_GAS_BOXES_HPP



struct [[maybe_unused]] cornell_box_gas_boxes_scene : public scene {
    cornell_box_gas_boxes_scene() : scene(1.0) {
        set_background(background_color::black);	//shouldn't matter -- can't see sky

        const auto red = std::make_shared<lambertian>(color(0.65, 0.05, 0.05));
        const auto white = std::make_shared<lambertian>(color(0.73, 0.73, 0.73));
        const auto green = std::make_shared<lambertian>(color(0.12, 0.45, 0.15));
        const auto light = std::make_shared<diffuse_light>(color(7, 7, 7));	//very bright light

        world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));	//left wall
        world.add(make_shared<yz_rect>(0, 555, 0, 555, 0  , red  ));	//right wall

        world.add(make_shared<flip_face>(make_shared<xz_rect>(113, 443, 127, 432, 554, light)));	//small light on roof

        world.add(make_shared<xz_rect>(0, 555, 0, 555, 0  , white));	//floor
        world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));	//roof

        world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));	//back wall
        //world.add(make_shared<xy_rect>(0, 555, 0, 555, 0  , white));	//front wall


        //big box
        std::shared_ptr<hittable> box1 = std::make_shared<box>(point3(0,0,0), point3(165,330,165), white);
        box1 = make_shared<rotate_y>(box1, 15);
        box1 = make_shared<translate>(box1, vec3(265, 0, 295));
        world.add(make_shared<constant_isotropic_medium>(box1, 0.01, color(0,0,0)) );


        //small box
        std::shared_ptr<hittable> box2 = std::make_shared<box>(point3(0,0,0), point3(165,165,165), white);
        box2 = std::make_shared<rotate_y>(box2, -18);
        box2 = std::make_shared<translate>(box2, vec3(130,0,65));
        world.add(make_shared<constant_isotropic_medium>(box2, 0.01, color(1,1,1)));


        set_camera(vec3(278, 278, -800), vec3(278, 278, 0), 40.0, 0.0);
    }
};

#endif //RAYTRACER_CORNELL_BOX_GAS_BOXES_HPP
