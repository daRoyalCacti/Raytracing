#ifndef RAYTRACER_PDF_H
#define RAYTRACER_PDF_H

#include "ONB.h"
#include "probability.h"
#include "hittable.h"


struct pdf {
    virtual ~pdf() = default;

    [[nodiscard]] virtual double value(const vec3& incoming_dir, const vec3& out_direction) const = 0;
    [[nodiscard]] virtual vec3 generate(const vec3& incoming_dir) const = 0;
};


struct cosine_pdf : public pdf {
    onb uvw;

    explicit cosine_pdf(const vec3& w) {
        uvw.build_from_w(w);
    }

    [[nodiscard]] double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        const auto cosine = dot(out_direction.normalized(), uvw.w());
        return cosine <= 0 ? 0 : cosine/pi;
    }

    [[nodiscard]] vec3 generate(const vec3& incoming_dir) const override {
        return uvw.local(random_cosine_direction());
    }
};


struct hittable_pdf : public pdf {
    point3 o;
    shared_ptr<hittable> ptr;

    hittable_pdf(const shared_ptr<hittable> &p, const point3& origin) : ptr(p), o(origin) {}

    [[nodiscard]] double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        return ptr->pdf_value(o, out_direction);
    }

    [[nodiscard]] vec3 generate(const vec3& incoming_dir) const override {
        return ptr->random(o);
    }
};



struct mixture_pdf : public pdf {
    shared_ptr<pdf> p[2];

    mixture_pdf(const shared_ptr<pdf> &p0, const shared_ptr<pdf> &p1) {
        p[0] = p0;
        p[1]= p1;
    }

    [[nodiscard]] double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        return 0.5 * p[0]->value(incoming_dir, out_direction) + 0.5 * p[1]->value(incoming_dir, out_direction);
    }

    [[nodiscard]] vec3 generate(const vec3& incoming_dir) const override {
        if (random_double() < 0.5)
            return p[0]->generate(incoming_dir);
        else
            return p[1]->generate(incoming_dir);
    }
};

struct Henyey_Greensteing_pdf : public pdf {
    const double g;

    explicit Henyey_Greensteing_pdf(const double g1) : g(g1) {}

    [[nodiscard]] double value(const vec3& incoming_dir, const vec3& out_direction) const override {
        //http://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions.html#PhaseHG
        return Henyey_Greensteing_pdf_func(g, dot(incoming_dir, out_direction));
    }

    //returns value in spherical coordinates
    [[nodiscard]] vec3 generate(const vec3& incoming_dir) const override {
        //https://math.stackexchange.com/questions/1418262/given-a-vector-and-angle-find-new-vector
        const auto angle = rand_Henyey_Greensteing(g);

        //select random vector to rotate around
        const auto rot = random_unit_vector();

        //rotate about the random vector by the angle
        return rotate(incoming_dir, rot, angle);
    }
};



#endif //RAYTRACER_PDF_H
