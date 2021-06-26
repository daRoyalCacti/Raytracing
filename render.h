#pragma once

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

template<size_t image_width, size_t image_height, size_t samplespp>
struct render {
    const scene curr_scene;
    static constexpr unsigned max_depth = 50;

    render() = delete;
    explicit render(scene scn) : curr_scene(std::move(scn)) {}

    //image width and height are what they say they are
    //samplespp in the number of samples initially to generate
    void draw_on_convergence(const std::string output, const double tol = 0.001) const {
        std::vector <std::vector<size_t>> samples;
        std::vector <std::vector<size_t>> curr_samples;     //keeps track of how many rays have been sent for a given pixel

        std::vector <std::vector<color>> buffer;   //standard buffer
        std::vector <std::vector<color>> buffer_write; //buffer that has been properly normalized for file writing
        std::vector <std::vector<color>> buffer_prev;  //stores the previous buffer (for convergence checking)

        samples.resize(image_width);
        curr_samples.resize(image_width);
        buffer.resize(image_width);
        buffer_write.resize(image_width);
        buffer_prev.resize(image_width);


        for (unsigned i = 0; i < image_width; i++) {
            samples[i].resize(image_height);
            curr_samples[i].resize(image_height);
            buffer[i].resize(image_height);
            buffer_write[i].resize(image_height);
            buffer_prev[i].resize(image_height);
            for (unsigned j = 0; j < image_height; j++) {
                samples[i][j] = samplespp;
                curr_samples[i][j] = 0;
            }
        }


        draw_to_buffer(buffer, samples);
        for (int j = (int) image_height - 1; j >= 0; --j) {
            for (unsigned i = 0; i < image_width; ++i) {
                buffer_prev[i][j] = buffer[i][j];
                curr_samples[i][j] += samples[i][j];
            }
        }

        bool should_quit = false;
        unsigned counter = 0;
        double max_dev = 0;
        size_t pixel_x, pixel_y;
        while (!should_quit) {
            max_dev = 0;
            should_quit = true;
            draw_to_buffer(buffer, samples);
            for (int j = (int) image_height - 1; j >= 0; --j) {
                for (unsigned i = 0; i < image_width; ++i) {
                    curr_samples[i][j] += samples[i][j];
                    buffer_write[i][j] = buffer[i][j] / (double)curr_samples[i][j];
                    //if the colour has not changed significatly
                    const auto dev_x = abs(buffer_write[i][j].x() - buffer_prev[i][j].x()/(double)curr_samples[i][j]);
                    const auto dev_y = abs(buffer_write[i][j].y() - buffer_prev[i][j].y()/(double)curr_samples[i][j]);
                    const auto dev_z = abs(buffer_write[i][j].z() - buffer_prev[i][j].z()/(double)curr_samples[i][j]);
                    if (dev_x > max_dev) {max_dev = dev_x; pixel_x = i; pixel_y = j;};
                    if (dev_y > max_dev) {max_dev = dev_y; pixel_x = i; pixel_y = j;};
                    if (dev_z > max_dev) {max_dev = dev_z; pixel_x = i; pixel_y = j;};

                    if ( ( dev_x > tol ) || ( dev_y > tol ) || ( dev_z > tol ) ) {
                        should_quit = false;    //must keep computing
                    } else {
                        samples[i][j] = 0;  //pixel is good enough and so we can save computation power but not sending any other rays to it
                    }
                    buffer_prev[i][j] = buffer[i][j];

                }
            }

            write_buffer_png(output, buffer_write);
            std::cout << "loop num : " << counter++ << "\tmax deviation : " << max_dev << "/" << tol << " at (" << pixel_x << "," << pixel_y << ")\n";
        }

    }


    //void draw_to_buffer(std::array <std::array<color, image_width>, image_height> &buffer,
    //                    const std::array <std::array<size_t, image_width>, image_height> samples) const {
    void draw_to_buffer(std::vector <std::vector<color>> &buffer,
                        const std::vector<std::vector<size_t>> samples) const {
        #pragma omp parallel for shared(buffer)
        for (int j = (int) image_height - 1; j >= 0; --j) {
            for (unsigned i = 0; i < image_width; ++i) {
                for (int s = 0; s < samples[i][j]; ++s) {
                    const auto u = double(i + random_double()) / (image_width - 1);
                    const auto v = double(j + random_double()) / (image_height - 1);
                    const ray r = curr_scene.cam.get_ray(u, v);
                    buffer[i][j] += ray_color(r, max_depth);
                }
            }
        }

    }

    [[nodiscard]] color ray_color(const ray &r, const unsigned depth) const {
        //collision with any object
        hit_record rec;

        //If we've reach the bounce limit, no more light is gathered
        if (depth <= 0)
            return color(0, 0, 0);

        //If the ray hits nothing, return the background color
        if (!curr_scene.world.hit(r, 0.001, infinity, rec))
            return curr_scene.background;


        if (curr_scene.settings.has_fog) {
            const auto fog_hit_time = curr_scene.fog->generate_hit_time();
            if (fog_hit_time < rec.t) {     //ray hit fog
                const auto scatter_pos = r.at(fog_hit_time);


                if (curr_scene.settings.importance) {
                    const auto light_ptr = make_shared<hittable_pdf>(curr_scene.settings.important, scatter_pos);
                    mixture_pdf mixed_pdf(light_ptr, curr_scene.fog->prob_density);

                    const auto scattered = ray(scatter_pos, mixed_pdf.generate(r.dir), r.time());
                    const auto pdf_val = mixed_pdf.value(r.dir, scattered.direction());

                    bool nothing_broke = true;
                    if (!std::isfinite(pdf_val)) {
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
        const color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v,
                                                   rec.p);    //is black if the material doesn't emit

        scatter_record srec;
        if (!rec.mat_ptr->scatter(r, rec, srec))    //if the light shouldn't scatter
            return emitted;

        if (srec.is_specular)
            return srec.attenuation * ray_color(srec.specular_ray, depth - 1);

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

            return emitted +
                   srec.attenuation * ray_color(scattered, depth - 1) * rec.mat_ptr->scattering_pdf(r, rec, scattered) /
                   pdf_val;    //return the color of the object darkened by the number of times the ray bounced

        }
    }


};