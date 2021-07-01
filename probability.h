#ifndef RAYTRACER_PROBABILITY_H
#define RAYTRACER_PROBABILITY_H

#include <cmath>
#include "vec3.h"
#include "vec2.h"


#include "Halton.hpp"

namespace global {  //must be declared for probability.h
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    std::mt19937 generator;
}


inline double random_double() {
    //Returns number from U[0,1)
    return global::distribution(global::generator);
}

inline double random_double(const double min, const double max) {
    //Returns number from U[min, max)
    return min + (max-min)*random_double();
}

inline int random_int(const int min, const int max) {
    //Returns a random integer from U[min,  max]
    return static_cast<int>(random_double(min, max+1));
}


inline static vec3 random_vec3() {	//static because does not need a particular vec3
    return vec3(random_double(), random_double(), random_double());
}

inline static vec3 random_vec3(const double min, const double max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}


inline vec3 random_in_unit_sphere() {
    while (true) {
        const auto p = random_vec3(-1,1);
        if (p.length_squared() < 1) return p;
    }
}



inline vec3 random_in_unit_disk() {
    while (true) {
        const auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1) return p;
    }
}



inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}


[[maybe_unused]] inline double exp_cdf(double x, double lambda) {
    return 1 - exp(-lambda*x);
}

inline double rand_exp(double lambda) {
    return -1/lambda*log(random_double());
}

inline double rand_exp_halton(double lambda, size_t &index) {
    return -1/lambda*log(random_halton_1D(index));
}


//returns a random angle
inline double rand_Henyey_Greensteing(double g) {
    //https://www.oceanopticsbook.info/view/scattering/level-2/the-henyey-greenstein-phase-function
    const double max_y = 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*fabs(g), 3/2.0f));
    while (true) {
        const auto x = random_double(-1, 1);
        const auto y = random_double(0, max_y);
        if (y < Henyey_Greensteing_pdf_func(g, x)) {
            return acos(x);
        }
    }
}

inline vec3 random_cosine_direction() {
    const auto r1 = random_double();
    const auto r2 = random_double();

    const auto z = sqrt(1-r2);
    const auto phi = 2*M_PI*r1;

    const auto x = cos(phi) * sqrt(r2);
    const auto y = sin(phi) * sqrt(r2);

    return vec3(x,y,z);
}

inline vec3 random_to_sphere(const double& radius, const double &distance_squared) {
    const auto r1 = random_double();
    const auto r2 = random_double();

    const auto z = 1 + r2* ( sqrt(1 - radius*radius/distance_squared) - 1);
    const auto phi = 2*M_PI*r1;

    if (z > 1)
        std::cerr << "random_to_sphere failed" << std::endl;

    const auto x = cos(phi)*sqrt(1-z*z);
    const auto y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}


#endif //RAYTRACER_PROBABILITY_H
