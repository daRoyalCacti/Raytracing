
#ifndef RAYTRACER_FOG_HPP
#define RAYTRACER_FOG_HPP

#include <functional>
#include "pdf.hpp"

struct participating_medium {
    const std::shared_ptr<pdf> prob_density;
    participating_medium() = delete;
    explicit participating_medium(std::shared_ptr<pdf> prob) : prob_density(std::move(prob)) {}
    [[nodiscard]] virtual double generate_hit_time() {return 0;}
    [[nodiscard]] virtual color color_at(const vec3 &hit_pos) const {
        return vec3(1,0,0);
    }

};

struct basic_constant_fog : public participating_medium {
    const double lambda;
    const color col;
    const double g;
    size_t halton_index = 0;

    basic_constant_fog(const color c, const double l, const double g_) : lambda(l), col(c), g(g_), participating_medium(std::make_shared<Henyey_Greensteing_pdf>(g)) {}

    [[nodiscard]] inline double generate_hit_time() override {
        return rand_exp_halton(lambda, halton_index);
    }


    [[nodiscard]] inline color color_at(const vec3 &hit_pos) const override {
        return col;
    }

};

#endif //RAYTRACER_FOG_HPP
