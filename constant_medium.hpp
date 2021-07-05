#pragma once

#include "helpful.hpp"
#include "hittable.hpp"
#include "material.hpp"

/*==================================================================================
The medium should always be the first object sampled in a scene
    - This is so the ray's t_max value can be updated first
      and so increase the efficiency of collisions with the scene
 =================================================================*/
struct constant_medium : public hittable {
	const std::shared_ptr<hittable> boundary;   //the boundary which the medium is contained in
	const std::shared_ptr<material> phase_function; //is a material but only using the scatter function
	const double neg_inv_density;		//required to move info from constructor to hit
	size_t halton_index = 0;
	
	//d for density
	constant_medium(std::shared_ptr<hittable> b, const double d, std::shared_ptr<material> mat)
		: boundary(std::move(b)), neg_inv_density(-1/d), phase_function(std::move(mat)) {};

	bool hit(const ray& r, double t_min, double t_max, hit_record& rec) override;

	bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		return boundary->bounding_box(time0, time1, output_box);
	}
};


//algorithm only works for convex shape
bool constant_medium::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) {
	hit_record rec1, rec2;
	
	//if the ray ever hits the boundary
	if (!boundary->hit(r, -infinity, infinity, rec1))
		return false;

	//if the ray hits the boundary after every hitting the boundary (i.e. the time found above)
	// - so the ray is on a trajectory that will enter the medium and exit it
	if (!boundary->hit(r, rec1.t+0.0001, infinity, rec2))
		return false;
	//note that finding these allows for the ray to move backwards in time
	// - i.e. if the ray starts inside the medium, this will not trivially return false

	if (rec1.t < t_min) rec1.t = t_min;	//if the entry collision happens before the min time
	if (rec2.t > t_max) rec2.t = t_max;	//if the exit collision happens after the max time
	//this is to only consider the ray moving between these times
	// - required when comparing the time the ray hit the medium
	// -- if the hit time is outside [t_min, t_max] we want to return no hit
	// -- same reason as for why we want to return false if the hit time happens outside the boundary of the medium

	if (rec1.t >= rec2.t)	//if the entry happens after or at the same time as the exit
		return false;

	if (rec1.t < 0) //required to find the distance inside the boundary
		rec1.t = 0; //(if negative, this distance would measure the distance based on where the ray came from, not where it is now)

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	//const auto hit_distance = neg_inv_density * log(random_halton_1D(halton_index));	//randomly deciding if the ray should leave the medium
	                                                                                        // - is choosing from an exponential distribution
	                                                                                        //from STAT2003, in general we take -1/lambda * ln(U) for U~U[0,1]
	                                                                                        //This is the same algorithm used in rand_exp_halton
    const auto hit_distance = neg_inv_density * log(random_double());   //using the halton sequence gives lines through the image
                                                                        // - not sure why

	if (hit_distance > distance_inside_boundary)	//if the randomly chosen distance is greater than the distance from the boundary to the ray origin
		return false;                               // of it if happens outside of the interval [t_min, t_max]

	rec.t = rec1.t + hit_distance / ray_length;	//t_f = t_i + d/s
	rec.p = r.at(rec.t);

	rec.normal = vec3(1,0,0);	//arbitrary
	rec.front_face = true;		//arbitrary
	rec.mat_ptr = phase_function;

	return true;
}


struct constant_isotropic_medium : public constant_medium {
    //d for density
    constant_isotropic_medium(const std::shared_ptr<hittable> &b, const double d, const std::shared_ptr<texture> &a)
            : constant_medium(b, d, std::make_shared<isotropic>(a)) {}

    constant_isotropic_medium(const std::shared_ptr<hittable> &b, const double d, const color c)
            : constant_medium(b, d, std::make_shared<isotropic>(c))  {}

};


struct constant_Henyey_medium : public constant_medium {
    //d for density
    constant_Henyey_medium(const std::shared_ptr<hittable> &b, const double d, const double g, const std::shared_ptr<texture> &a)
            : constant_medium(b, d, std::make_shared<Henyey_Greensteing>(a, g)) {}

    constant_Henyey_medium(const std::shared_ptr<hittable> &b, const double d, const double g, const color c)
            : constant_medium(b, d, std::make_shared<Henyey_Greensteing>(c, g))  {}

};