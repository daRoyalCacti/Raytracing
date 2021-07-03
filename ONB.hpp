#ifndef RAYTRACER_ONB_HPP
#define RAYTRACER_ONB_HPP

#include "vec3.hpp"

//ONB = orthonormal basis
struct onb {
    const vec3 axis[3];

    onb() = delete;
    explicit onb(const vec3& n) : axis{unit_vector(n),
                                    unit_vector(cross(  unit_vector(n), fabs(unit_vector(n).x()) > 0.9 ? vec3(0,1,0) : vec3(1, 0, 0)  )),
                                    cross(unit_vector(n), unit_vector(cross(  unit_vector(n), fabs(unit_vector(n).x()) > 0.9 ? vec3(0,1,0) : vec3(1, 0, 0)  )  )) }
    {}

    inline vec3 operator[](int i) const {return axis[2-i];};    //the positioning of uvw in the array changed, hence the "2-"

    [[nodiscard]] inline vec3 u() const {return axis[2]; }
    [[nodiscard]] inline vec3 v() const {return axis[1]; }
    [[nodiscard]] inline vec3 w() const {return axis[0]; }

    [[maybe_unused]] [[nodiscard]] inline vec3 local(double a, double b, double c) const {
        return a*u() + b*v() + c*w();
    }

    [[nodiscard]] inline vec3 local(const vec3& a) const {
        return a.x()*u() + a.y()*v() + a.z()*w();
    }


};

#endif //RAYTRACER_ONB_HPP
