#ifndef RAYTRACER_PDF_HPP
#define RAYTRACER_PDF_HPP

#include "hittable.hpp"
#include "ONB.hpp"
#include "probability.hpp"


struct pdf {
    virtual ~pdf() = default;

    [[nodiscard]] virtual double value(const vec3& incoming_dir, const vec3& out_direction) const = 0;
    [[nodiscard]] virtual vec3 generate(const vec3& incoming_dir) = 0;
};


//pdf used by lambertian materials
struct cosine_pdf : public pdf {
    const onb uvw;
    //size_t halton_index = 0;  //cannot make work (see below)

    explicit cosine_pdf(const vec3& w) : uvw(w){}

    [[nodiscard]] inline double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        const auto cosine = dot(unit_vector(out_direction), uvw.w());
        return cosine <= 0 ? 0 : cosine/pi;
    }

    [[nodiscard]] inline vec3 generate(const vec3& incoming_dir) override {
        return uvw.local(random_cosine_direction());    //would likely be better the halton set was used but
                                                        // using it leads to some very strange images
    }
};

//used for importance sampling hittables
struct hittable_pdf : public pdf {
    const point3 o;
    const std::shared_ptr<hittable> ptr;

    hittable_pdf() = delete;
    hittable_pdf(std::shared_ptr<hittable> p, const point3& origin) : ptr(std::move(p)), o(origin) {}

    [[nodiscard]] double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        return ptr->pdf_value(o, out_direction);
    }

    [[nodiscard]] vec3 generate(const vec3& incoming_dir) override {
        return ptr->random(o);
    }
};



struct mixture_pdf : public pdf {
    const std::shared_ptr<pdf> p[2];

    mixture_pdf() = delete;
    mixture_pdf(const std::shared_ptr<pdf> &p0, const std::shared_ptr<pdf> &p1) : p{p0, p1} {}

    [[nodiscard]] inline double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        return 0.5 * p[0]->value(incoming_dir, out_direction) + 0.5 * p[1]->value(incoming_dir, out_direction);
    }

    [[nodiscard]] inline vec3 generate(const vec3& incoming_dir) override {
        if (random_double() < 0.5)
            return p[0]->generate(incoming_dir);
        else
            return p[1]->generate(incoming_dir);
    }
};

struct Henyey_Greensteing_pdf : public pdf {
    const double g;
    size_t halton_index1 = 0,hatlon_index2 = 0; //  //does not work - see below

    explicit Henyey_Greensteing_pdf(const double g1) : g(g1) {}

    [[nodiscard]] inline double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        //http://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions.html#PhaseHG
        const double cos_theta = dot(incoming_dir, out_direction);
        return 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*g*cos_theta, 3/2.0f));
    }

    //returns value in spherical coordinates
    [[nodiscard]] inline vec3 generate(const vec3& incoming_dir) override {
        const auto angle = rand_Henyey_Greensteing_halton(g, halton_index1);

        //select random vector to rotate around
        const auto rot = random_unit_vector_halton(hatlon_index2);

        //rotate about the random vector by the angle
        return rotate(incoming_dir, rot, angle);
    }
};



#endif //RAYTRACER_PDF_HPP
