//
// Created by jacob on 30/6/21.
//

#ifndef RAYTRACER_HALTON_HPP
#define RAYTRACER_HALTON_HPP

#include <utility>
#include <array>
#include <cmath>
#include <algorithm>    //for std::shuffle
#include "vec2.hpp"
#include "vec3.hpp"


#define run_time_Halton_set //must be defined above the include
// also must be a define (rather than a constexpr) by the implementation
namespace global {  //must be declared before Halton.hpp is included
    constexpr size_t halton_sequence_stored_size = 1e4;     //the number of elements to store in the Halton sequence
}

namespace Halton {
#ifdef run_time_Halton_set
//https://en.wikipedia.org/wiki/Halton_sequence
    template<unsigned base>
    //templated to align with the compile time version
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

#endif

#ifndef run_time_Halton_set
    //https://en.wikipedia.org/wiki/Halton_sequence
    constexpr size_t halton_sequence_size = global::halton_sequence_stored_size;

    template <size_t Index, size_t Base, size_t counter>
    struct Halton {
        static constexpr size_t i = Halton<Index, Base, counter+1>::i/ Base;
        static constexpr double f = Halton<Index, Base, counter+1>::f/Base;
        static constexpr double r = (Halton<Index, Base, counter+1>::i>0) ? (Halton<Index, Base, counter+1>::r + f*(Halton<Index, Base, counter+1>::i % Base)) : Halton<Index, Base, counter+1>::r;
    };


    template <size_t Index, size_t Base>
    struct Halton<Index, Base, 100> {   //some testing showed that for the first million numbers, only a depth of 20 was required --- 100 then should be plenty
        static constexpr size_t i = Index;
        static constexpr double f = 1;
        static constexpr double r = 0;
    };

    template <size_t Index, size_t Base>
    struct Halton_i {
        static constexpr size_t i = Halton<Index, Base, 1>::i;
        static constexpr double f = Halton<Index, Base, 1>::f;
        static constexpr double r = Halton<Index, Base, 1>::r;
    };


    //for making the numbers accessible at runtime
    template<size_t Base, size_t ... I>
    double Halton_impl(std::index_sequence<I...>, size_t i) {
        constexpr std::array<double, sizeof...(I)> a = {Halton_i<I, Base>::r...};

        return a[i];
    }
    template<size_t Base>
    double get_Halton(size_t i) {
        return Halton_impl<Base>(std::make_index_sequence<halton_sequence_size+1>(), i);
    }
#endif

}

template <size_t N>
struct Halton_set {
    std::array<double, N> halton_set_1D;
    std::array<vec2, N> halton_set_2D;
    std::array<vec3, N> halton_set_3D;

    //only taking every <leap> value of the sequence
    // can improve convergence, see https://en.wikipedia.org/wiki/Halton_sequence
    static constexpr unsigned leap = 409;

    Halton_set() {
        for (unsigned i = 0; i < N; i++) {
            halton_set_1D[i] = Halton::get_halton<2>(leap*i);
            halton_set_2D[i] = vec2(halton_set_1D[i], Halton::get_halton<3>(leap*i));
            halton_set_3D[i] = vec3(halton_set_2D[i], Halton::get_halton<5>(leap*i));
        }
    }
};

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
inline double Henyey_Greensteing_pdf_func(double g, double cos_theta) {
    return 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*g*cos_theta, 3/2.0f));
}


//returns a random angle
inline double rand_Henyey_Greensteing_halton(double g, size_t &index) {
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
                throw std::runtime_error("Infinite loop in rand_Henyey_Greensteing_halton");
            }
        #endif
        const auto r = random_halton_2D(-1, 1, 0, max_y, index);
        if (r.y() < Henyey_Greensteing_pdf_func(g, r.x())) {
            return acos(r.x());
        }
    }
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


inline vec3 random_unit_vector_halton(size_t &index) {
    return unit_vector(halton_random_in_unit_sphere(index));    //random value must be from unit sphere else there would be
                                                                    // a larger probability of the random vector pointing towards
                                                                    // the corner of the cube (sample from U([a,b]x[c.d]) is from a rectangular prism)
}

inline vec3 random_to_sphere_halton(double radius, double distance_squared, size_t &index) {
    const auto r = random_halton_2D(index);

    const auto z = 1 + r.y()* ( sqrt(1 - radius*radius/distance_squared) - 1);
    const auto phi = 2*M_PI*r.x();

    if (z > 1)
        std::cerr << "random_to_sphere failed" << std::endl;

    const auto x = cos(phi)*sqrt(1-z*z);
    const auto y = sin(phi)*sqrt(1-z*z);

    return vec3(x, y, z);
}

inline double rand_exp_halton(double lambda, size_t &index) {
    return -1/lambda*log(random_halton_1D(index));
}


#endif //RAYTRACER_HALTON_HPP
