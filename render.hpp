#pragma once

#include "scenes.hpp"
#include "color.hpp"

//for creating a directory
#include <bits/stdc++.h> 
#include <sys/stat.h> 
#include <sys/types.h>

constexpr bool log_scanlines = false;  //prints the number of scalines remaining when rendering an image

//image width and height are what they say they are
template<size_t image_width, size_t image_height>
struct render {
    scene curr_scene;
    static constexpr unsigned max_depth = 50;

    std::vector<std::vector<size_t>> halton_indices;    //used to get random numbers from the halton sequence

    render() = delete;
    explicit render(scene scn) : curr_scene(std::move(scn)) {
        halton_indices.resize(image_width);
        for (unsigned i = 0; i < image_width; i++) {
            halton_indices[i].resize(image_height);
            for (unsigned j = 0; j < image_height; j++) {
                halton_indices[i][j] = 0;
            }
        }
    }


    //samplespp in the number of samples initially to generate
    template <size_t samplespp>
    void draw_on_convergence(const std::string &output, const double tol = 0.1) {
        //======================================
        //The general idea:
        // - consider the plot of the number of rays per pixel against the color of the pixel (or some component of color, say red)
        // - then consider the plot of the derivative of this
        // - when the plot of the derivative reaches tol, we quit
        // Basically, we keep on adding more rays into the convergence gets signficantly slow
        //======================================
        constexpr long unsigned total_rays = samplespp*image_width*image_height;

        std::cout << "Initialising render";
        const auto start_init = std::chrono::high_resolution_clock::now();
        std::vector <std::vector<size_t>> samples;
        std::vector <std::vector<size_t>> curr_samples;     //keeps track of how many rays have been sent for a given pixel

        std::vector <std::vector<color>> buffer;   //standard buffer
        std::vector <std::vector<color>> buffer_write; //buffer that has been properly normalized for file writing
        std::vector <std::vector<color>> buffer_prev;  //stores the previous buffer (for convergence checking)

        std::vector <std::vector<double>> conv;  //stores how fast each pixel is converging to the solution

        samples.resize(image_width);
        curr_samples.resize(image_width);
        buffer.resize(image_width);
        buffer_write.resize(image_width);
        buffer_prev.resize(image_width);
        conv.resize(image_width);


        for (unsigned i = 0; i < image_width; i++) {
            samples[i].resize(image_height);
            curr_samples[i].resize(image_height);
            buffer[i].resize(image_height);
            buffer_write[i].resize(image_height);
            buffer_prev[i].resize(image_height);
            conv[i].resize(image_height);
            for (unsigned j = 0; j < image_height; j++) {
                samples[i][j] = samplespp;
                curr_samples[i][j] = 0;
            }
        }
        const auto end_init = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed_seconds_init = end_init - start_init;
        std::cout << " -- took " << elapsed_seconds_init.count() << "s" << std::endl;


        std::cout << "Generating initial image" << std::flush;
        const auto start_image1 = std::chrono::high_resolution_clock::now();
        draw_to_buffer(buffer, samples);
        const auto end_image1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed_seconds_image1 = end_image1 - start_image1;
        std::cout << " -- took " << elapsed_seconds_image1.count() << "s" << std::endl;

        std::cout << "\tupdating buffers" << std::flush;
        const auto start_buffer1 = std::chrono::high_resolution_clock::now();
        for (int j = (int) image_height - 1; j >= 0; --j) {
            for (unsigned i = 0; i < image_width; ++i) {
                buffer_prev[i][j] = buffer[i][j];
                curr_samples[i][j] += samples[i][j];
                buffer_write[i][j] = buffer[i][j] / (double)curr_samples[i][j];
            }
        }
        const auto end_buffer1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed_seconds_buffer1 = end_buffer1 - start_buffer1;
        std::cout << " -- took " << elapsed_seconds_buffer1.count() << "s" << std::endl;

        std::cout << "\twriting to disk";
        const auto start_disk1 = std::chrono::high_resolution_clock::now();
        write_buffer_png(output, buffer_write);
        const auto end_disk1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed_seconds_disk1 = end_disk1 - start_disk1;
        std::cout << " -- took " << elapsed_seconds_disk1.count() << "s" << std::endl;

        bool should_quit = false;
        unsigned counter = 0;
        double max_dev, max_dev_l;  //storing the quickest convergence and the color with the quickest convergence
        double sum; //summing over all max_dev_l --- acts as a normalising factor for redistributing the rays
        while (!should_quit) {
            max_dev = 0;
            sum = 0;
            should_quit = true;
            counter++;

            std::cout << "Generating image " << counter  << std::flush;
            const auto start_image = std::chrono::high_resolution_clock::now();
            draw_to_buffer(buffer, samples);
            const auto end_image = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsed_seconds_image = end_image - start_image;
            std::cout << " -- took " << elapsed_seconds_image.count() << "s" << std::endl;

            std::cout << "\tupdating buffers"  << std::flush;
            const auto start_buffer = std::chrono::high_resolution_clock::now();
            for (int j = (int) image_height - 1; j >= 0; --j) {
                for (unsigned i = 0; i < image_width; ++i) {
                    curr_samples[i][j] += samples[i][j];
                    buffer_write[i][j] = buffer[i][j] / (double)curr_samples[i][j];
                    //determining how much each the colour of each pixel has changed by a repeated iteration (for a single photon)
                    const auto dev_x = abs(buffer_write[i][j].x() - buffer_prev[i][j].x()/(double)curr_samples[i][j]) / samples[i][j];
                    const auto dev_y = abs(buffer_write[i][j].y() - buffer_prev[i][j].y()/(double)curr_samples[i][j]) / samples[i][j];
                    const auto dev_z = abs(buffer_write[i][j].z() - buffer_prev[i][j].z()/(double)curr_samples[i][j]) / samples[i][j];
                    max_dev_l = dev_x;
                    if (dev_y > max_dev_l) {max_dev_l = dev_y;}
                    if (dev_z > max_dev_l) {max_dev_l = dev_z;}
                    conv[i][j] = max_dev_l;
                    sum += max_dev_l;

                    if (max_dev_l > max_dev) {max_dev = max_dev_l;}   //for printing purposes

                    //if the current pixel is converging too fast, then we must keep iterating
                    if ( max_dev_l > tol ) {
                        should_quit = false;
                    }

                    buffer_prev[i][j] = buffer[i][j];
                }
            }
            const auto end_buffer = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsed_seconds_buffer = end_buffer - start_buffer;
            std::cout << " -- took " << elapsed_seconds_buffer.count() << "s" << std::endl;
            std::cout << "\tmax deviation : " << max_dev << "/" << tol << std::endl;


            std::cout << "\tredistributing rays" << std::flush;
            const auto start_redis = std::chrono::high_resolution_clock::now();
            //redistributing the rays based on the convergence
            for (int j = (int) image_height - 1; j >= 0; --j) {
                for (unsigned i = 0; i < image_width; ++i) {
                    samples[i][j] = ceil(total_rays * conv[i][j] / sum);
                    if (samples[i][j]==0) {
                        samples[i][j] = 1;
                    }
                }
            }
            const auto end_redis = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsed_seconds_redis = end_redis - start_redis;
            std::cout << " -- took " << elapsed_seconds_redis.count() << "s" << std::endl;



            std::cout << "\twriting to disk" << std::flush;
            const auto start_disk = std::chrono::high_resolution_clock::now();
            write_buffer_png(output, buffer_write);
            const auto end_disk = std::chrono::high_resolution_clock::now();
            const std::chrono::duration<double> elapsed_seconds_disk = end_disk - start_disk;
            std::cout << " -- took " << elapsed_seconds_disk.count() << "s" << std::endl;

        }

    }


    void draw_to_buffer(std::vector <std::vector<color>> &buffer,
                        const std::vector<std::vector<size_t>> &samples) {
        unsigned counter = 0;
        #pragma omp parallel for shared(buffer, counter, samples), default(none)
        for (int j = (int) image_height - 1; j >= 0; --j) {
            if constexpr (log_scanlines) {
                std::cout << "\rScanline: " << ++counter << " / " << image_height << std::flush;
            }
            for (unsigned i = 0; i < image_width; ++i) {
                for (int s = 0; s < samples[i][j]; ++s) {
                    const auto r_v = random_halton_2D(halton_indices[i][j]);
                    const auto u = double(i + r_v.x()) / (image_width - 1);
                    const auto v = double(j + r_v.y()) / (image_height - 1);
                    const ray r = curr_scene.cam->get_ray(u, v);
                    buffer[i][j] += ray_color(r, max_depth);
                }
            }
        }
        if constexpr (log_scanlines) {
            std::cout << "\r                                  \n" << std::flush;
        }
    }

    [[nodiscard]] color ray_color(const ray &r, const unsigned depth) {
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