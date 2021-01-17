#ifndef RAYTRACER_PDF_H
#define RAYTRACER_PDF_H

#include "ONB.h"
#include "probability.h"
#include "hittable.h"


struct pdf {
    virtual ~pdf() {}

    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};


struct cosine_pdf : public pdf {
    onb uvw;

    cosine_pdf(const vec3& w) {
        uvw.build_from_w(w);
    }

    virtual double value(const vec3& direction) const override {
        const auto cosine = dot(unit_vector(direction), uvw.w());
        return cosine <= 0 ? 0 : cosine/pi;
    }

    virtual vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }
};


struct hittable_pdf : public pdf {
    point3 o;
    shared_ptr<hittable> ptr;

    hittable_pdf(const shared_ptr<hittable> &p, const point3& origin) : ptr(p), o(origin) {}

    virtual double value(const vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }

    virtual vec3 generate() const override {
        //return vec3(1,1,1);
        return ptr->random(o);
    }
};



struct mixture_pdf : public pdf {
    shared_ptr<pdf> p[2];

    mixture_pdf(const shared_ptr<pdf> &p0, const shared_ptr<pdf> &p1) {
        p[0] = p0;
        p[1]= p1;
    }

    virtual double value(const vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    virtual vec3 generate() const override {
        if (random_double() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }
};


struct Henyey_Greensteing_pdf : public pdf {
    double g[2];
    const vec3 incoming_dir;

    Henyey_Greensteing_pdf(const double g1 , const double g2, const vec3 &inc_dir) : incoming_dir(inc_dir) {
        g[0] = g1;
        g[1] = g2;
    }

    virtual double value(const vec3& direction) const override {
        //turning direction into spherical coordinates
        const auto spherical = cartesian_to_spherical(direction);

        //returning the values from Henyey_Greensteing_pdf_func (product of the 2)
        return Henyey_Greensteing_pdf_func(g[0], spherical[1]) * Henyey_Greensteing_pdf_func(g[1], spherical[2]);
    }

    //returns value in spherical coordinates
    virtual vec3 generate() const override {
        const auto theta = rand_Henyey_Greensteing(g[0]);
        const auto phi = rand_Henyey_Greensteing(g[1]);

        auto spherical = cartesian_to_spherical(incoming_dir);

        spherical[1] += theta;
        spherical[2] += phi;

        return spherical_to_cartesian(spherical);
    }
};

#endif //RAYTRACER_PDF_H
