
#ifndef RAYTRACER_FOG_H
#define RAYTRACER_FOG_H

#include <functional>
#include "material.h"
#include "pdf.h"

struct participating_medium {
    shared_ptr<pdf> prob_density;
    virtual double generate_hit_time() const {return 0;}
     virtual color color_at(const vec3 &hit_pos) const {return vec3(0,0,0);}
};

struct basic_constant_fog : public participating_medium {
    const double lambda;
    const color col;
    basic_constant_fog(const color c, const double l, const double g1, const double g2) : lambda(l), col(c) {
        prob_density = make_shared<Henyey_Greensteing_pdf>(g1, g2);
    }

    virtual double generate_hit_time() const override {
        return rand_exp(lambda);
    }

    virtual color color_at(const vec3 &hit_pos) {
        return col;
    }

};

    /*
typedef std::function<bool (const ray, const vec3, scatter_record&)> fog_func;

//https://www.astro.umd.edu/~jph/HG_note.pdf
//https://blog.demofox.org/2014/06/22/analytic-fog-density/
//https://computergraphics.stackexchange.com/questions/227/how-are-volumetric-effects-handled-in-raytracing
bool basic_fog(const ray start, const vec3 end, scatter_record &srec) {
    const double lambda = 0.0015;//0.05;  //determines how often particles scatter - 'density of the fog'
    const double g1 = 0.3, g2 = 0.4;   //determines how particles scatter
    const auto length = (start.orig-end).length();

    const auto pos = rand_exp(lambda);

    if (pos > length)   //the ray should scatter after the ray has collided
        return false;

    const auto theta = rand_Henyey_Greensteing(g1);
    const auto phi = rand_Henyey_Greensteing(g2);
    srec.is_specular = true;
    srec.specular_ray = ray(start.at(pos), vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(phi)), start.tm + pos);
    return true;
}

bool no_fog(const ray start, const vec3 end, scatter_record &srec) {
    return false;
}
*/
#endif //RAYTRACER_FOG_H
