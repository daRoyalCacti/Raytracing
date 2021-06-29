//ONB = orthonormal basis

#ifndef RAYTRACER_ONB_H
#define RAYTRACER_ONB_H

struct onb {
    const vec3 axis[3];

    onb() = delete;
    explicit onb(const vec3& n) : axis{unit_vector(n),
                                    unit_vector(cross(  unit_vector(n), fabs(unit_vector(n).x()) > 0.9 ? vec3(0,1,0) : vec3(1, 0, 0)  )),
                                    cross(unit_vector(n), unit_vector(cross(  unit_vector(n), fabs(unit_vector(n).x()) > 0.9 ? vec3(0,1,0) : vec3(1, 0, 0)  )  )) }
    {
        /*
         //has old uvw positi0n in array
            axis[2] = unit_vector(n);
            const vec3 a = fabs(w().x()) > 0.9 ? vec3(0,1,0) : vec3(1, 0, 0);
            axis[1] = unit_vector(cross(w(), a));
            axis[0] = cross(w(), v());
        */
                                    }

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

#endif //RAYTRACER_ONB_H
