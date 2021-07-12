//
// Created by jacob on 13/7/21.
//

#ifndef RAYTRACER_MESH_SCENES_HPP
#define RAYTRACER_MESH_SCENES_HPP


struct [[maybe_unused]] door_scene : public scene {
    door_scene() : scene(aspec1) {
        set_background(background_color::sky);	//shouldn't matter -- can't see sky

        world.add(generate_model("../models/door/door.obj"));
        world.add(std::make_shared<sphere>(vec3(0, -100, -1), 100, std::make_shared<lambertian>(vec3(0,1,0)) ));


        set_camera(vec3(-3, 4, -5), vec3(0, 1, 0), 20.0, 0.0);
    }
};


struct [[maybe_unused]] cup_scene : public scene {
    cup_scene() : scene(aspec1) {
        set_background(background_color::sky);	//shouldn't matter -- can't see sky

        world.add(generate_model("../models/cup/cup.obj"));
        world.add(std::make_shared<sphere>(vec3(0, -100.5, -1), 100, std::make_shared<lambertian>(vec3(0,1,0)) ));


        set_camera(vec3(0, 0, -0.5), vec3(0, 0, 0), 20.0, 0.0);
    }
};


struct [[maybe_unused]] crate_scene : public scene {
    crate_scene() : scene(aspec1) {
        set_background(background_color::sky);	//shouldn't matter -- can't see sky

        world.add(generate_model("../models/crate/Crate1.obj"));
        world.add(std::make_shared<sphere>(vec3(0, -100.5, -1), 100, std::make_shared<lambertian>(vec3(0,1,0)) ));


        set_camera(vec3(-3, 4, -5), vec3(0, 1, 0), 20.0, 0.0);
    }
};


#endif //RAYTRACER_MESH_SCENES_HPP
