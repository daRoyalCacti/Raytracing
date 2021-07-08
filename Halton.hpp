//
// Created by jacob on 30/6/21.
//

#ifndef RAYTRACER_HALTON_HPP
#define RAYTRACER_HALTON_HPP

#include <utility>
#include <array>
#include "vec2.hpp"
#include "vec3.hpp"


namespace global {  //must be declared before Halton.hpp is included
    constexpr size_t halton_sequence_stored_size = 1e7;     //the number of elements to store in the Halton sequence
}


//https://en.wikipedia.org/wiki/Halton_sequence
template <unsigned base>
double get_halton(size_t index) {
    double f = 1;
    double r = 0;

    while (index > 0) {
        f = f / base;
        r = r + f * static_cast<double>(index % base);
        index = static_cast<size_t>(index / base);
    }

    return r;
}

template <size_t N>
struct Halton_set {
    std::array<double, N> halton_set_1D;    //cannot be initialised with {} else compiling uses too much ram
    std::array<vec2, N> halton_set_2D;
    std::array<vec3, N> halton_set_3D;

    bool is_initialised = false;

    //only taking every <leap> value of the sequence
    // can improve convergence, see https://en.wikipedia.org/wiki/Halton_sequence
    static constexpr unsigned leap = 409;

    void init() {
        if (!is_initialised) {
            for (unsigned i = 0; i < N; i++) {
                halton_set_1D[i] = get_halton<2>(leap * i);
                halton_set_2D[i] = vec2(halton_set_1D[i], get_halton<3>(leap * i));
                halton_set_3D[i] = vec3(halton_set_2D[i], get_halton<5>(leap * i));
            }
            is_initialised = true;
        }
    }
};

//might be a better way to store the halton sequences that in global memory
namespace global {
    Halton_set<halton_sequence_stored_size> Halton_rng;
}



inline double random_halton_1D(size_t &index) {
    const double ret_val = global::Halton_rng.halton_set_1D[index];
    index = (index + 1) % global::halton_sequence_stored_size;
    return ret_val;
}

inline double random_halton_1D(double min, double max, size_t &index) {
    return min + (max-min)*random_halton_1D(index);
}


inline vec2 random_halton_2D(size_t &index) {
    const vec2 ret_val = global::Halton_rng.halton_set_2D[index];
    index = (index + 1) % global::halton_sequence_stored_size;

    return ret_val;
}

//gets a pseudo-random value from [min,max]x[min,max]
inline vec2 random_halton_2D(double min, double max, size_t &index) {
    return min + (max-min)*random_halton_2D(index);
}

//gets a pseudo-random value from [min1,max1]x[min2,max2]
inline vec2 random_halton_2D(double min1, double max1, double min2, double max2, size_t &index) {
    auto r = random_halton_2D(index);
    return vec2( (max1-min1)*r.x() + min1, (max2-min2)*r.y() + min2);
}


inline vec3 random_halton_3D(size_t &index) {
    const vec3 ret_val = global::Halton_rng.halton_set_3D[index];
    index = (index + 1) % global::halton_sequence_stored_size;
    return ret_val;
}

//gets a pseudo-random value from [min,max]
inline vec3 random_halton_3D(double min, double max, size_t &index) {
    return min + (max-min)*random_halton_3D(index);
}



inline int random_int_halton(int min, int max, size_t &index) {
    //Returns a random integer from U[min,  max]
    return static_cast<int>(random_halton_1D(min, max+1, index));
}

//Does not work --- gives very strange images if used
// - not entirely sure why
/*
inline vec3 random_cosine_direction_halton(size_t &index) {
    //const auto r1 = random_double();
    //const auto r2 = random_double();
    const auto r = random_halton_2D(index);

    const auto z = sqrt(1-r.y());
    const auto phi = 2*M_PI*r.x();

    const auto x = cos(phi) * sqrt(r.y());
    const auto y = sin(phi) * sqrt(r.y());

    return vec3(x,y,z);
}*/


//https://www.astro.umd.edu/~jph/HG_note.pdf
//https://www.desmos.com/calculator/84ypsmsy95
//https://www.oceanopticsbook.info/view/scattering/level-2/the-henyey-greenstein-phase-function
//returns a random angle
//on sampling of scattering phase functions -- J. Zhang
inline double rand_Henyey_Greensteing_halton(const double g, size_t &index) {
    const auto r = random_halton_1D(index);
    const double mu = 1/ (2*g) * (1 + g*g - ( (1-g*g) / (1-g+2*g*r) )*( (1-g*g) / (1-g+2*g*r) )  );
    return acos(mu);
}



inline vec3 halton_random_in_unit_sphere(size_t &index) {
    #ifndef NDEBUG
        unsigned counter = 0;
        constexpr auto max_loops = 100;
    #endif
    while (true) {
        #ifndef NDEBUG
            ++counter;
            if (counter > max_loops) {
                throw std::runtime_error("Infinite loop in halton_random_in_unit_sphere");
            }
        #endif
        const vec3 p = random_halton_3D(-1,1, index);
        if (p.length_squared() < 1) return p;
    }
}

inline vec3 halton_random_in_unit_disk(size_t &index) {
    #ifndef NDEBUG
        unsigned counter = 0;
        constexpr auto max_loops = 100;
    #endif
    while (true) {
        #ifndef NDEBUG
            ++counter;
            if (counter > max_loops) {
                throw std::runtime_error("Infinite loop in halton_random_in_unit_disk");
            }
        #endif
        const auto p = vec3(random_halton_2D(-1, 1, index), 0);
        if (p.length_squared() < 1) return p;
    }
}


//https://mathworld.wolfram.com/SpherePointPicking.html
inline vec3 random_unit_vector_halton(size_t &index) {
    vec2 r;
    while (true) {
        r = random_halton_2D(-1, 1, index);
        if (r.x() * r.x() + r.y() * r.y() < 1) break;
    }

    const auto x1 = r.x();
    const auto x2 = r.y();

    const auto x12_p_x22 = x1 * x1 + x2 * x2;

    return vec3(2 * x1 * sqrt(1 - x12_p_x22), 2 * x2 * sqrt(1 - x12_p_x22), 1 - 2 * x12_p_x22);
}

inline vec3 random_to_sphere_halton(double radius, double distance_squared, size_t &index) {
    const auto r = random_halton_2D(index);

    const auto z = 1 + r.y()* ( sqrt(1 - radius*radius/distance_squared) - 1);
    const auto phi = 2*M_PI*r.x();

#ifndef NDEBUG
    if (z > 1)
        std::cerr << "random_to_sphere failed" << std::endl;
#endif

    const auto x = cos(phi)*sqrt(1-z*z);
    const auto y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}

[[maybe_unused]] inline double rand_exp_halton(double lambda, size_t &index) {
    return -1/lambda*log(random_halton_1D(index));
}


#endif //RAYTRACER_HALTON_HPP
