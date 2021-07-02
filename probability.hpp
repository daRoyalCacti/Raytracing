#ifndef RAYTRACER_PROBABILITY_HPP
#define RAYTRACER_PROBABILITY_HPP

#include <cmath>
#include "vec3.hpp"
#include "vec2.hpp"


#include "Halton.hpp"


inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    //Returns number from U[0,1)
    return distribution(generator);
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
    #ifndef NDEBUG
        unsigned counter = 0;
        constexpr auto max_loops = 100;
    #endif
    while (true) {
        #ifndef NDEBUG
            ++counter;
            if (counter > max_loops) {
                throw std::runtime_error("Infinite loop in random_in_unit_sphere");
            }
        #endif
        const auto p = random_vec3(-1,1);
        if (p.length_squared() < 1) return p;
    }
}



[[maybe_unused]] inline vec3 random_in_unit_disk() {
    #ifndef NDEBUG
        unsigned counter = 0;
        constexpr auto max_loops = 100;
    #endif
    while (true) {
        #ifndef NDEBUG
            ++counter;
            if (counter > max_loops) {
                throw std::runtime_error("Infinite loop in random_in_unit_disk");
            }
        #endif
        const auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1) return p;
    }
}



[[maybe_unused]] inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}


[[maybe_unused]] inline double exp_cdf(double x, double lambda) {
    return 1 - exp(-lambda*x);
}

[[maybe_unused]] inline double rand_exp(double lambda) {
    static std::exponential_distribution<> distribution(lambda);
    static std::mt19937 generator;
    //Returns number from U[0,1)
    return distribution(generator);
    //return -1/lambda*log(random_double());
}




//returns a random angle
[[maybe_unused]] inline double rand_Henyey_Greensteing(double g) {
    //https://www.oceanopticsbook.info/view/scattering/level-2/the-henyey-greenstein-phase-function
    const double max_y = 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*fabs(g), 3/2.0f));
    #ifndef NDEBUG
        unsigned counter = 0;
        constexpr auto max_loops = 100;
    #endif
    while (true) {
        #ifndef NDEBUG
            ++counter;
            if (counter > max_loops) {
                throw std::runtime_error("Infinite loop in rand_Henyey_Greensteing");
            }
        #endif
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

[[maybe_unused]] inline vec3 random_to_sphere(const double& radius, const double &distance_squared) {
    const auto r1 = random_double();
    const auto r2 = random_double();

    const auto z = 1 + r2* ( sqrt(1 - radius*radius/distance_squared) - 1);
    const auto phi = 2*M_PI*r1;

    #ifndef NDEBUG
        if (z > 1)
            std::cerr << "random_to_sphere failed" << std::endl;
    #endif

    const auto x = cos(phi)*sqrt(1-z*z);
    const auto y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}


#endif //RAYTRACER_PROBABILITY_HPP
