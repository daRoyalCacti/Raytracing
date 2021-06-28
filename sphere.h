#pragma once

#include "hittable.h"
#include "vec3.h"
#include "material.h"

struct sphere : public hittable {
	const point3 center = vec3(0,0,0);
	double radius = 0;
	const shared_ptr<material> mat_ptr;

	sphere() = default;
	sphere(const point3 cen, const double r, const shared_ptr<material>& m): center(cen), radius(r), mat_ptr(m) {}

	bool hit(const ray&r, double t_min, double t_max, hit_record& rec) const override;
	bool bounding_box(double time0, double time1, aabb& output_box) const override;

    [[nodiscard]] double pdf_value(const point3& o, const vec3& v) const override;
    [[nodiscard]] vec3 random(const point3& o) const override;

	private:
	static void get_sphere_uv(const point3& p, double& u, double& v) {
		//p: a given point on a unit sphere
		//u: normalised angle around Y axis (starting from x=-1)
		//v: normalised angle around Z axis (from Y=-1 to Y=1)
		// - normalised means in [0,1] as is standard for texture coordinates

		const auto theta = acos(-p.y());			//theta and phi in standard spherical coordinates
		const auto phi = atan2(-p.z(), p.x()) + pi;	//techically phi = atan2(p.z, -p.x) but this is discontinuous
								// - this uses atan2(a, b) = atan2(-a,-b) + pi which is continuous
								// - atan2(a,b) = atan(a/b)

		u = phi / (2 * pi);	//simple normalisation
		v = theta / pi;
	}
};

bool sphere::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
	//using the quadratic equation to find if (and when) 'ray' collides with sphere centred at 'center' with radius 'radius'
	
	const vec3 oc = r.origin() - center;
	const auto a = r.direction().squaredNorm();
	const auto half_b =  dot(oc, r.direction());	//uses half b to simplify the quadratic equation
	const auto c = oc.squaredNorm() - radius * radius;
	const auto discriminant = half_b*half_b - a*c;

	if (discriminant < 0) return false;	//if there is no collision
	const auto sqrtd = sqrt(discriminant);

	//Find the nearest root that lies in the acceptable range.
	const auto root1 = (-half_b - sqrtd) / a;	//first root
	if (root1 < t_min || t_max < root1) {	//if the first root is ouside of the accepctable range
		//if true, check the second root
		const auto root2 = (-half_b + sqrtd) / a;
		if (root2 < t_min || t_max < root2) {
            //if the second root is ouside of the range, there is no hit
            return false;
        } else {
		    rec.t = root2;
		}
	} else {
        rec.t = root1;	//root is finding time
	}


#ifndef NDEBUG
	if (!std::isfinite(rec.t)) std::cout << "sphere collision gave infinite time" << std::endl;
#endif
	rec.p = r.at(rec.t);
	const vec3 outward_normal = (rec.p - center) / radius;	//a normal vector is just a point on the sphere less the center
								//dividing by radius to make it normalised
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;

	get_sphere_uv(outward_normal, rec.u, rec.v);	//setting the texture coordinates
							//outward_normal is technical a vec3 not a point3 but they are the same thing
							// - it points to the correct position on a unit sphere

	return true;	//the ray collides with the sphere
}

bool sphere::bounding_box(const double time0, const double time1, aabb& output_box) const {
	output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}


double sphere::pdf_value(const point3& o, const vec3& v) const {
    hit_record rec;
    if (!this->hit(ray(o,v), 0.001, infinity, rec))
        return 0;

    const auto cos_theta_max = sqrt(1 - radius*radius/(center-o).squaredNorm());
    const auto solid_angle = 2*pi*(1 - cos_theta_max);

    #ifndef NDEBUG
    [[unlikely]] if (!std::isfinite(1/solid_angle) ) {
	    std::cerr << "sphere pdf_value failed\nsolid_angle=" << solid_angle << std::endl;

	    if (!std::isfinite(solid_angle) ) {
	        std::cerr << "\twould be because cos_theta_max=" << cos_theta_max << std::endl;

            if (!std::isfinite(solid_angle) ) {
                std::cerr << "\t\twould be becuase";
                if ((center-o).length_squared() == 0) {
                    std::cerr << " (center-o).length_squared = " << (center-o).length_squared() << " is not 0" << std::endl;
                }
                if (radius*radius/(center-o).length_squared() >=1) {
                    std::cerr << " (center-o).length() = " << (center-o).length() << " is less than " << radius << " (the ray is inside the sphere)" << std::endl;
                }

            }
	    }

	    std::cerr << "sphere is centered at (" << center << ") ray origin is at (" << o << ") with the sphere radius being " << radius << std::endl;
    }
    #endif

    return 1 / solid_angle;
}

vec3 sphere::random(const point3& o) const {
    const vec3 direction = center - o;
    const double distance_squared = direction.squaredNorm();

    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local( random_to_sphere(radius, distance_squared) );
}
