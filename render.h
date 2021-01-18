#pragma once

#include <functional>

#include "common.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "scenes.h"

#include <fstream>
#include <utility>

//for creating a directory
#include <bits/stdc++.h> 
#include <sys/stat.h> 
#include <sys/types.h>



struct render {
	unsigned image_width, image_height;
	unsigned samples_per_pixel;	//number of light rays per pixel
	unsigned max_depth = 50;	//max number of light bounces
	unsigned number_of_temp;
	const scene curr_scene;

	explicit render(scene scn, const unsigned img_w, const unsigned samples = 1000, const unsigned temp = 10) :
			image_width(img_w), samples_per_pixel(samples), number_of_temp(temp), curr_scene(std::move(scn)) {
        image_height = static_cast<int>(image_width / curr_scene.aspect_ratio);
	}


	void draw() const {
		//creating a temp directory
		std::string rand_str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		std::random_device rd;	
		std::mt19937 generator(rd());

		std::shuffle(rand_str.begin(), rand_str.end(), generator);

		const std::string temp_file_dir = "./temp_image_dir_" + rand_str.substr(0, 15);

		mkdir(temp_file_dir.c_str(), 0777);

		for (int n = 0; n < number_of_temp; n++) {
			std::ofstream out;
			out.open(temp_file_dir + "/" + std::to_string(n) + ".ppm");


			out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

			for (int j = (int)image_height-1; j>=0; --j) {
				std::cout << "\rScanlines remaining: " << j+1 << " / " << image_height << " of image " << n+1 << " / " << number_of_temp << std::flush;
				for (unsigned i = 0; i < image_width; ++i) {
					color pixel_color(0,0,0);
					//#pragma omp parallel for default(none) shared(i, j, pixel_color)
					for (int s = 0; s < samples_per_pixel; ++s) {
						const auto u = double(i + random_double()) / (image_width-1);
						const auto v = double(j + random_double()) / (image_height-1);
						const ray r = curr_scene.cam.get_ray(u, v);
						pixel_color += ray_color(r, (int)max_depth);
					}
					write_color(out, pixel_color, samples_per_pixel);
				}
			}
			out.close();
			std::cerr << "\r" << std::flush;
		}

		average_images(temp_file_dir, "image.png");

		//deleting the temp images
		std::filesystem::path pathToDelete(temp_file_dir);
		remove_all(pathToDelete);
	}

    [[nodiscard]] color ray_color(const ray& r, const int depth) const {
        //collision with any object
        hit_record rec;

        //If we've reach the bounce limit, no more light is gathered
        if (depth <= 0)
            return color(0,0,0);

        //If the ray hits nothing, return the background color
        if (!curr_scene.world.hit(r, 0.001, infinity, rec))
            return curr_scene.background;





        if (curr_scene.settings.has_fog) {
            const auto fog_hit_time = curr_scene.fog->generate_hit_time();
            //if (fog_hit_time > rec.t - r.tm) {    //ray hit fog
            if (fog_hit_time < rec.t) {     //ray hit fog
                const auto scatter_pos = r.at(fog_hit_time);

                //curr_scene.fog->create_pdf(r.dir);


                if (curr_scene.settings.importance) {
                    const auto light_ptr = make_shared<hittable_pdf>(curr_scene.settings.important, scatter_pos);
                    mixture_pdf mixed_pdf(light_ptr, curr_scene.fog->prob_density);

                    const auto scattered = ray(scatter_pos, mixed_pdf.generate(r.dir), r.time());
                    const auto pdf_val = mixed_pdf.value(r.dir, scattered.direction());

                    bool nothing_broke = true;
                    if(!std::isfinite(pdf_val)) {
                        nothing_broke = false;  //is not always cause for erroring out
                                                // - when importance sampling a dielectric sphere,
                                                //   the generated ray is moving through the sphere.
                                                //   Because the ray is inside the sphere, the
                                                //   importance sampling fails.
                                                //   However, it should fail because there is
                                                //   no fog inside the sphere
                    }


                    if (nothing_broke) {
                        return curr_scene.fog->color_at(scatter_pos) * ray_color(scattered, depth - 1) *
                               curr_scene.fog->prob_density->value(r.dir, scattered.direction()) / pdf_val;
                    }

                }
                //else
                return curr_scene.fog->color_at(scatter_pos) *
                           ray_color(ray(scatter_pos, curr_scene.fog->prob_density->generate(r.dir), r.time()),
                                     depth - 1);

            }


        }






        //else keep bouncing light
        const color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);	//is black if the material doesn't emit

        scatter_record srec;
        if (!rec.mat_ptr->scatter(r, rec, srec))	//if the light shouldn't scatter
            return emitted;

        if (srec.is_specular)
            return srec.attenuation * ray_color(srec.specular_ray, depth-1);

        if (curr_scene.settings.importance) {
            //https://en.wikipedia.org/wiki/Monte_Carlo_integration#Importance_sampling
            const auto light_ptr = make_shared<hittable_pdf>(curr_scene.settings.important, rec.p);
            mixture_pdf mixed_pdf(light_ptr, srec.pdf_ptr);

            const auto scattered = ray(rec.p, mixed_pdf.generate(r.dir), r.time());
            const auto pdf_val = mixed_pdf.value(r.dir, scattered.direction());


            return emitted +
                   srec.attenuation * ray_color(scattered, depth - 1) * rec.mat_ptr->scattering_pdf(r, rec, scattered) /
                   pdf_val;    //return the color of the object darkened by the number of times the ray bounced
        } else {
            const auto scattered = ray(rec.p, srec.pdf_ptr->generate(r.dir), r.time());
            const auto pdf_val = srec.pdf_ptr->value(r.dir, scattered.direction());

            return emitted + srec.attenuation * ray_color(scattered, depth-1) * rec.mat_ptr->scattering_pdf(r, rec, scattered) / pdf_val;	//return the color of the object darkened by the number of times the ray bounced

        }
    }
};

