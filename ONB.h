//ONB = orthonormal basis

#ifndef RAYTRACER_ONB_H
#define RAYTRACER_ONB_H

struct onb {
    vec3 axis[3];

    onb() = default;

    inline vec3 operator[](int i) const {return axis[i];};

    [[nodiscard]] vec3 u() const {return axis[0]; }
    [[nodiscard]] vec3 v() const {return axis[1]; }
    [[nodiscard]] vec3 w() const {return axis[2]; }

    [[maybe_unused]] [[nodiscard]] inline vec3 local(double a, double b, double c) const {
        return a*u() + b*v() + c*w();
    }

    [[nodiscard]] inline vec3 local(const vec3& a) const {
        return a.x()*u() + a.y()*v() + a.z()*w();
    }

    inline void build_from_w(const vec3& n) {
        axis[2] = n.normalized();
        const vec3 a = fabs(w().x()) > 0.9 ? vec3(0,1,0) : vec3(1, 0, 0);
        axis[1] = cross(w(), a).normalized();
        axis[0] = cross(w(), v());
    }
};

#endif //RAYTRACER_ONB_H
