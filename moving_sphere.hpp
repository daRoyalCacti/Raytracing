#pragma once

#include "hittable.hpp"

//a way to add motion blur to a sphere
struct moving_sphere : public hittable {
	const point3 center0;	//centres of spheres at time0 (at time1 not needed --- stored in dc_d_dt)
	const double time0 = 0, time1 = 0;
	const double radius = 0;
	const std::shared_ptr<material> mat_ptr;
	const vec3 dc_d_dt;    //(centre1 - centre0)/(time1 - time0)

	moving_sphere() = delete;
	moving_sphere(const point3 cen0, const point3 cen1, const double _time0, const double _time1, const double r, std::shared_ptr<material> m) :
		center0(cen0), time0(_time0), time1(_time1), radius(r), mat_ptr(std::move(m)), dc_d_dt( (cen1-cen0) / (_time1-_time0)) {};

	bool hit_time(const ray&r, double t_min, double t_max, hit_record& rec) override;
    void hit_info(const ray&r, double t_min, double t_max, hit_record& rec) override;

	bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	[[nodiscard]] inline point3 center(const double time) const {
		return center0 + (time - time0) * dc_d_dt;
	}

    private:
    static inline void get_sphere_uv(const point3& p, double& u, double& v) {
        //p: a given point on a unit sphere
        //u: normalised angle around Y axis (starting from x=-1)
        //v: normalised angle around Z axis (from Y=-1 to Y=1)
        // - normalised means in [0,1] as is standard for texture coordinates

        const auto theta = acos(-p.y());			//theta and phi in standard spherical coordinates
        const auto phi = atan2(-p.z(), p.x()) + pi;	//techically phi = atan2(p.z, -p.x) but this is discontinuous
        // - this uses atan2(a, b) = atan2(-a,-b) + pi which is continuous
        // - atan2(a,b) = atan(a/b)

        u = phi / two_pi;	//simple normalisation
        v = theta / pi;
    }
};


bool moving_sphere::hit_time(const ray& r, const double t_min, const double t_max, hit_record& rec) {
	//essentially the same as sphere::hit but center is now center(time)
	const vec3 oc = r.origin() - center(r.time());  //vector for the origin to the center of the sphere
	const auto a = r.direction().length_squared();
	const auto half_b =  dot(oc, r.direction());	//uses half b to simplify the quadratic equation
	const auto c = oc.length_squared() - radius * radius;
	const auto discriminant = half_b*half_b - a*c;

	if (discriminant < 0) return false;	//if there is no collision
	const auto sqrtd = sqrt(discriminant);

    //Find the nearest root that lies in the acceptable range.
    const auto root1 = (-half_b - sqrtd) / a;	//first root
    if (root1 < t_min || t_max < root1) {	//if the first root is outside of the acceptable range
        //if true, check the second root
        const auto root2 = (-half_b + sqrtd) / a;
        if (root2 < t_min || t_max < root2) {
            //if the second root is outside of the range, there is no hit
            return false;
        } else {
            rec.t = root2;
        }
    } else {
        rec.t = root1;	//root is finding time
    }
#ifndef NDEBUG
    if (!std::isfinite(rec.t)) std::cout << "moving sphere collision gave infinite time" << std::endl;
#endif

	return true;	//the ray collides with the sphere
	
}

void moving_sphere::hit_info(const ray& r, const double t_min, const double t_max, hit_record& rec) {
    rec.p = r.at(rec.t);
    const vec3 outward_normal = (rec.p - center(r.time())) / radius;	//a normal vector is just a point on the sphere less the center
    //dividing by radius to make it normalised
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    //get_sphere_uv(outward_normal, rec.u, rec.v);	//setting the texture coordinates
    //outward_normal is technical a vec3 not a point3 but they are the same thing
    // - it points to the correct position on a unit sphere
}

bool moving_sphere::bounding_box(const double _time0, const double _time1, aabb& output_box) const {
	aabb box0(center(_time0) - vec3(radius,radius,radius), center(_time0) + vec3(radius,radius,radius));
	aabb box1(center(_time1) - vec3(radius,radius,radius), center(_time1) + vec3(radius,radius,radius));
	output_box = surrounding_box(box0, box1);
	return true;
}
