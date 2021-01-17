#ifndef RAYTRACER_PROBABILITY_H
#define RAYTRACER_PROBABILITY_H

#include <math.h>
#include "vec3.h"

inline double exp_cdf(double x, double lambda) {
    return 1 - exp(-lambda*x);
}

inline double rand_exp(double lambda) {
    return -1/lambda*log(random_double());
}

//https://www.astro.umd.edu/~jph/HG_note.pdf
//https://www.desmos.com/calculator/84ypsmsy95
inline double Henyey_Greensteing_pdf_func(double g, double theta) {
    return 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*g*cos(theta), 3/2.0f));
}

inline double rand_Henyey_Greensteing(double g) {
    const double max_y = 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*fabs(g), 3/2.0f));
    while (true) {
        const auto x = random_double(0, 2 * M_PI);
        const auto y = random_double(0, max_y);
        if (y < Henyey_Greensteing_pdf_func(g, x)) {
            return x;
        }
    }
}

inline vec3 random_cosine_direction() {
    const auto r1 = random_double();
    const auto r2 = random_double();

    const auto z = sqrt(1-r2);
    const auto phi = 2*pi*r1;

    const auto x = cos(phi) * sqrt(r2);
    const auto y = sin(phi) * sqrt(r2);

    return vec3(x,y,z);
}

inline vec3 random_to_sphere(const double& radius, const double &distance_squared) {
    const auto r1 = random_double();
    const auto r2 = random_double();

    const auto z = 1 + r2* ( sqrt(1 - radius*radius/distance_squared) - 1);
    const auto phi = 2*pi*r1;

    if (z > 1)
        std::cerr << "random_to_sphere failed" << std::endl;

    const auto x = cos(phi)*sqrt(1-z*z);
    const auto y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}


#endif //RAYTRACER_PROBABILITY_H
