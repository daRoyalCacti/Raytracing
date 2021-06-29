
#ifndef RAYTRACER_FOG_H
#define RAYTRACER_FOG_H

#include <functional>
#include "material.h"
#include "pdf.h"

struct participating_medium {
    const shared_ptr<pdf> prob_density;
    participating_medium() = delete;
    explicit participating_medium(const shared_ptr<pdf> &prob) : prob_density(prob) {}
    [[nodiscard]] virtual double generate_hit_time() const {return 0;}
    [[nodiscard]] virtual color color_at(const vec3 &hit_pos) const {
        return vec3(1,0,0);
    }

};

struct basic_constant_fog : public participating_medium {
    const double lambda;
    const color col;
    const double g;
    basic_constant_fog(const color c, const double l, const double g_) : lambda(l), col(c), g(g_), participating_medium(make_shared<Henyey_Greensteing_pdf>(g)) {}

    [[nodiscard]] inline double generate_hit_time() const override {
        return rand_exp(lambda);
    }


    [[nodiscard]] inline color color_at(const vec3 &hit_pos) const override {
        return col;
    }

};

#endif //RAYTRACER_FOG_H
