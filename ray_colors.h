
#ifndef RAYTRACER_RAY_COLORS_H
#define RAYTRACER_RAY_COLORS_H

#include <functional>
#include "hittable.h"
#include "vec3.h"
#include "fog.h"

typedef std::function<color (const ray&, const hittable&, const int, const color&, fog_func, shared_ptr<hittable>)> ray_func;

color ray_color_no_importance(const ray& r, const hittable& world, const int depth, const color& background, std::function<bool (const ray, const vec3, scatter_record&)> hit_fog, shared_ptr<hittable> lights) {
    //collision with any object
    hit_record rec;

    const color fog_color = color(0.8, 0.8, 0.8);

    //If we've reach the bounce limit, no more light is gathered
    if (depth <= 0)
        return color(0,0,0);

    //If the ray hits nothing, return the background color
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    scatter_record srec;
    if (hit_fog(r, rec.p, srec)) {
        return fog_color * ray_color_no_importance(srec.specular_ray, world, depth-1, background, hit_fog, lights);
    }


    //else keep bouncing light
    const color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);	//is black if the material doesn't emit


    if (!rec.mat_ptr->scatter(r, rec, srec))	//if the light shouldn't scatter
        return emitted;

    if (srec.is_specular)
        return srec.attenuation * ray_color_no_importance(srec.specular_ray, world, depth-1, background, hit_fog, lights);

    //const auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    //mixture_pdf mixed_pdf(light_ptr, srec.pdf_ptr);

    scattered = ray(rec.p, srec.pdf_ptr->generate(), r.time());
    const auto pdf_val = srec.pdf_ptr->value(scattered.direction());



    return emitted + srec.attenuation * ray_color_no_importance(scattered, world, depth-1, background, hit_fog, lights) * rec.mat_ptr->scattering_pdf(r, rec, scattered) / pdf_val;	//return the color of the object darkened by the number of times the ray bounced

}


color ray_color1(const ray& r, const hittable& world, const int depth, const color& background, std::function<bool (const ray, const vec3, scatter_record&)> hit_fog, shared_ptr<hittable> lights) {
    //collision with any object
    hit_record rec;

    const color fog_color = color(0.8, 0.8, 0.8);

    //If we've reach the bounce limit, no more light is gathered
    if (depth <= 0)
        return color(0,0,0);

    //If the ray hits nothing, return the background color
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    scatter_record srec;
    if (hit_fog(r, rec.p, srec)) {
        return fog_color * ray_color1(srec.specular_ray, world, depth-1, background, hit_fog, lights);
    }


    //else keep bouncing light
    const color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);	//is black if the material doesn't emit


    if (!rec.mat_ptr->scatter(r, rec, srec))	//if the light shouldn't scatter
        return emitted;

    if (srec.is_specular)
        return srec.attenuation * ray_color1(srec.specular_ray, world, depth-1, background, hit_fog, lights);

    //https://en.wikipedia.org/wiki/Monte_Carlo_integration#Importance_sampling
    const auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf mixed_pdf(light_ptr, srec.pdf_ptr);

    scattered = ray(rec.p, mixed_pdf.generate(), r.time());
    const auto pdf_val = mixed_pdf.value(scattered.direction());



    return emitted + srec.attenuation * ray_color1(scattered, world, depth-1, background, hit_fog, lights) * rec.mat_ptr->scattering_pdf(r, rec, scattered) / pdf_val;	//return the color of the object darkened by the number of times the ray bounced

}

#endif //RAYTRACER_RAY_COLORS_H
