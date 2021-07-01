//
// Created by jacob on 30/6/21.
//

#ifndef RAYTRACER_VEC2_H
#define RAYTRACER_VEC2_H

struct vec2 {
    double e[2];
    vec2() : e{0, 0}{}
    vec2(double e0, double e1) : e{e0, e1} {}

    [[nodiscard]] inline double x() const {return e[0];}
    [[nodiscard]] inline double y() const {return e[1];}
};

inline vec2 operator * (const double t, const vec2 &v) {
    return vec2(t*v.e[0], t*v.e[1]);
}

inline vec2 operator * (const vec2 &v, const double t) {
    return t * v;
}

inline vec2 operator + (const double t, const vec2 &v) {
    return vec2(t+v.e[0], t+v.e[1]);
}

inline vec2 operator + (const vec2 &v, const double t) {
    return t + v;
}

#endif //RAYTRACER_VEC2_H
