#ifndef RAYTRACER_PROBABILITY_HPP
#define RAYTRACER_PROBABILITY_HPP

#include <cmath>
#include <random>
#include "vec3.hpp"
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


inline vec3 random_vec3() {
    return vec3(random_double(), random_double(), random_double());
}

inline vec3 random_vec3(const double min, const double max) {
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline vec2 random_vec2() {	//static because does not need a particular vec3
    return vec2(random_double(), random_double());
}

inline vec2 random_vec2(const double min, const double max) {
    return vec2(random_double(min, max), random_double(min, max));
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
    vec2 r;
    while (true) {
        r = random_vec2(-1, 1);
        if (r.x() * r.x() + r.y() * r.y() < 1) break;
    }

    const auto x1 = r.x();
    const auto x2 = r.y();

    const auto x12_p_x22 = x1 * x1 + x2 * x2;

    return vec3(2 * x1 * sqrt(1 - x12_p_x22), 2 * x2 * sqrt(1 - x12_p_x22), 1 - 2 * x12_p_x22);
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
    const auto r = random_double();
    const double mu = 1/ (2*g) * (1 + g*g - ( (1-g*g) / (1-g+2*g*r) )*( (1-g*g) / (1-g+2*g*r) )  );
    return acos(mu);
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
