#pragma once

#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#define EIGEN_USE_MKL_ALL   //https://eigen.tuxfamily.org/dox/TopicUsingIntelMKL.html
                            //https://eigen.tuxfamily.org/dox/TopicUsingBlasLapack.html
#ifdef NDEBUG
    #define EIGEN_NO_DEBUG
#endif
using namespace Eigen;

using std::sqrt;

typedef Vector3d vec3;

using point3 = vec3;
using color = vec3;

template <typename T, typename U>
vec3 cross(const T &vec1, const U &vec2) {
    return vec1.cross(vec2);
}

template <typename T, typename U>
double dot(const T &vec1, const U &vec2) {
    return vec1.dot(vec2);
}

template <typename T>
vec3 unit_vector(const T &vec1) {
    return vec1.normalized();
}

//random vector with elements in range [0,1]
vec3 random_vec3() {
    return vec3(random_double(), random_double(), random_double());
}

vec3 random_vec3(const double a, const double b) {
    //return (b-a)*random_vec3() + vec3(a, a, a);
    return vec3(random_double(a, b), random_double(a, b), random_double(a, b));
}



vec3 random_in_unit_sphere() {
    while (true) {
        //const auto p = Eigen::Vector3d::Random();
        const auto p = random_vec3(-1,1);
        if (p.squaredNorm() < 1) return p;
    }
}

vec3 random_in_unit_disk() {
    while (true) {
        const auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.squaredNorm() < 1) return p;
    }
}

vec3 random_unit_vector() {
    return random_in_unit_sphere().normalized();
}

vec3 reflect(const vec3& v, const vec3& n) {
    //reflects a vector v about another vector n
    return v - 2 * dot(v,n)*n;
}

vec3 refract(const vec3& uv, const vec3& n, const double etai_over_etat) {
    //Computes the refracted ray of light passing through a dielectric material using snell's law
    const auto cos_theta = fmin(dot(-uv, n), 1.0);
    const vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    const vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.squaredNorm() )) * n;
    return r_out_perp + r_out_parallel;
}


//https://ksuweb.kennesaw.edu/~plaval//math4490/rotgen.pdf
vec3 rotate(const vec3& vec, const vec3 &axis, const double angle) {
    const auto r = axis.normalized();
    return (1-cos(angle))*dot(vec,r)*r   +  cos(angle)*vec   +  sin(angle)*cross(r, vec);
}


/*
struct vec3 {
	double e[3];
	vec3() : e{0,0,0} {}
	vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
	vec3(double E) : e{E,E,E} {}

	[[nodiscard]] double x() const {return e[0];}
	[[nodiscard]] double y() const {return e[1];}
	[[nodiscard]] double z() const {return e[2];}

	vec3 operator -() const {return vec3(-e[0], -e[1], -e[2]); }
	double operator [](int i) const {return e[i];}
	double& operator [](int i) {return e[i];}

	vec3& operator += (const vec3 &v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator *= (const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator /= (const double t) {
		return *this *= 1/t;
	}

	[[nodiscard]] inline double length() const {
		return sqrt(length_squared());
	}

	[[nodiscard]] inline double length_squared() const {
		return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
	}

	inline static vec3 random() {	//static because does not need a particular vec3
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(const double min, const double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

};


//Type aliases
using point3 = vec3;
using color = vec3;



//vec3 Utility Functions

inline std::ostream& operator << (std::ostream &out, const vec3 &v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator + (const vec3 &u, const vec3 &v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator - (const vec3 &u, const vec3 &v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator * (const vec3 &u, const vec3 &v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator * (const double t, const vec3 &v) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator * (const vec3 &v, const double t) {
	return t * v;
}

inline vec3 operator / (const vec3 &v, const double t) {
	return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
	return vec3( u.e[1] * v.e[2] - u.e[2] * v.e[1],  u.e[2] * v.e[0] - u.e[0] * v.e[2],  u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3 v) {
	return v / v.length();
}

inline vec3 random_in_unit_sphere() {
	while (true) {
		const auto p = vec3::random(-1,1);
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

inline vec3 reflect(const vec3& v, const vec3& n) {
	//reflects a vector v about another vector n
	return v - 2 * dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, const double etai_over_etat) {
	//Computes the refracted ray of light passing through a dielectric material using snell's law
	const auto cos_theta = fmin(dot(-uv, n), 1.0);	
	const vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
	const vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared() )) * n;
	return r_out_perp + r_out_parallel;
}


//https://ksuweb.kennesaw.edu/~plaval//math4490/rotgen.pdf
inline vec3 rotate(const vec3& vec, const vec3 &axis, const double angle) {
    const auto r = unit_vector(axis);
    return (1-cos(angle))*dot(vec,r)*r   +  cos(angle)*vec   +  sin(angle)*cross(r, vec);
}
 */