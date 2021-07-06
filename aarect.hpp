//axis aligned rectangle
#pragma once

#include "hittable.hpp"

struct xy_rect : public hittable {
	const std::shared_ptr<material> mp;
	const double x0 = 0, x1 = 0, y0 = 0, y1 = 0, k = 0;	//x's and y's define a rectangle in the standard way
					//k defines z position
	const double area = 0;  //the area of the rectangle -- used to save computation when computing the pdf_value
	const double lx = 0, ly = 0;    //the length of the box in the x and y directions
	                                // - used to save computation in computing the collision

	size_t halton_index = 0;

	xy_rect() = delete;
	xy_rect(const double _x0, const double _x1, const double _y0, const double _y1, const double _k, std::shared_ptr<material> mat)
		: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(std::move(mat)), area( (x1-x0)*(y1-y0)), lx(x1-x0), ly(y1-y0) {};

	bool hit_time(const ray& r, double t_min, double t_max, hit_record& rec) override;
    void hit_info(const ray& r, double t_min, double t_max, hit_record& rec) override;

	inline bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		//just padding the z direction by a small amount
		constexpr double small = 0.0001;
		output_box = aabb(point3(x0, y0, k-small), point3(x1, y1, k+small));
		return true;
	}

    [[nodiscard]] inline double pdf_value(const point3 &origin, const vec3 &v) override {
        hit_record rec;
        const auto temp_ray = ray(origin, v);
        if (!this->hit_time(temp_ray, 0.001, infinity, rec))
            return 0;
        this->hit_info(temp_ray, 0.001, infinity, rec);


        const auto v_length2 = v.length_squared();
        const double distance_squared = rec.t * rec.t * v_length2;
        const double cosine = fabs(dot(v, rec.normal)) / sqrt(v_length2);

        return distance_squared / (cosine * area);
    }

    [[nodiscard]] inline vec3 random(const point3 &origin) override {
        const vec3 random_point = point3(random_halton_2D(x0, x1, y0, y1, halton_index), k);
        return random_point - origin;
    }
};

//very similar to xy rect -- see for comments
struct xz_rect : public hittable {
	const std::shared_ptr<material> mp;
	const double x0 = 0, x1 = 0, z0 = 0, z1 = 0, k = 0;	//k defines y position
    const double area = 0;
	const double lx = 0, lz = 0;
	size_t halton_index = 0;

	xz_rect() = delete;
	xz_rect(const double _x0, const double _x1, const double _z0, const double _z1, const double _k, std::shared_ptr<material> mat)
		: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(std::move(mat)), area((x1-x0)*(z1-z0) ), lx(x1-x0), lz(z1-z0) {};

    bool hit_time(const ray& r, double t_min, double t_max, hit_record& rec) override;
    void hit_info(const ray& r, double t_min, double t_max, hit_record& rec) override;

    inline bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		constexpr double small = 0.0001;
		output_box = aabb(point3(x0, k-small, z1), point3(x1, k+small, z1));
		return true;
	}

	[[nodiscard]] inline double pdf_value(const point3 &origin, const vec3 &v) override {
	    hit_record rec;
        const auto temp_ray = ray(origin, v);
        if (!this->hit_time(temp_ray, 0.001, infinity, rec))
            return 0;
        this->hit_info(temp_ray, 0.001, infinity, rec);

        const auto v_length2 = v.length_squared();
	    const double distance_squared = rec.t * rec.t * v_length2;
	    const double cosine = fabs(dot(v, rec.normal)) / sqrt(v_length2);

	    return distance_squared / (cosine * area);
	}

	[[nodiscard]] inline vec3 random(const point3 &origin) override {
	    const auto r = random_halton_2D(x0, x1, z0, z1, halton_index);
        const auto random_point = point3(r.x(), k, r.y());
	    return random_point - origin;
	}
};

//very similar to xy rect -- see for comments
struct yz_rect : public hittable {
	const std::shared_ptr<material> mp;
	const double y0 = 0, y1 = 0, z0 = 0, z1 = 0, k = 0;	//k defines x position
    const double area = 0;
	const double lz = 0, ly = 0;
	size_t halton_index = 0;

	yz_rect() = delete;
	yz_rect(const double _y0, const double _y1, const double _z0, const double _z1, const double _k, std::shared_ptr<material> mat)
		: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(std::move(mat)), area( (z1-z0)*(y1-y0) ), lz(z1-z0), ly(y1-y0) {};

    bool hit_time(const ray& r, double t_min, double t_max, hit_record& rec) override;
    void hit_info(const ray& r, double t_min, double t_max, hit_record& rec) override;

    inline bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		constexpr double small = 0.0001;
		output_box = aabb(point3(k-small, y0, z0), point3(k+small, y1, z1));
		return true;
	}

    [[nodiscard]] inline double pdf_value(const point3 &origin, const vec3 &v) override {
        hit_record rec;
        const auto temp_ray = ray(origin, v);
        if (!this->hit_time(temp_ray, 0.001, infinity, rec))
            return 0;
        this->hit_info(temp_ray, 0.001, infinity, rec);

        const auto v_length2 = v.length_squared();
        const double distance_squared = rec.t * rec.t * v_length2;
        const double cosine = fabs(dot(v, rec.normal)) / sqrt(v_length2);

        return distance_squared / (cosine * area);
    }

    [[nodiscard]] inline vec3 random(const point3 &origin) override {
        const auto random_point = point3(k, random_halton_2D(y0, y1, z0, z1, halton_index));
        return random_point - origin;
    }
};


bool xy_rect::hit_time(const ray&r, const double t_min, const double t_max, hit_record& rec)  {
	const double t = (k-r.origin().z()) / r.direction().z();	//time of collision - see aabb.h for why this is the case

	if (t < t_min || t > t_max)	//if collision is outside of specified times
		return false;
	
	//the x and y components of the ray at the time of collision
	const double x = r.origin().x() + t*r.direction().x();
	const double y = r.origin().y() + t*r.direction().y();

	//the collision algorithm
	// - simply if the x and y components of the point of collision are outside the specified size of the rectangle
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;	//there was no collision

    rec.t = t;

	//normalise x and y to be used as texture coords
	rec.u = (x-x0)/lx;
	rec.v = (y-y0)/ly;


#ifndef NDEBUG
	if (!std::isfinite(rec.t)) std::cout << "xy_rec collision gave infinite time" << std::endl;
#endif

	return true;
}

void xy_rect::hit_info(const ray&r, const double t_min, const double t_max, hit_record& rec) {
    const auto outward_normal = vec3(0, 0, 1);	//the trivial normal vector
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(rec.t);
}



bool xz_rect::hit_time(const ray&r, const double t_min, const double t_max, hit_record& rec) {
	const auto t = (k-r.origin().y()) / r.direction().y();	//time of collision - see aabb.h for why this is the case

	if (t < t_min || t > t_max)	//if collision is outside of specified times
		return false;
	
	//the x and y components of the ray at the time of collision
	const auto x = r.origin().x() + t*r.direction().x();
	const auto z = r.origin().z() + t*r.direction().z();

	//the collision algorithm
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;	//there was no collision
	
	//normalise x and z to be used as texture coords
	rec.u = (x-x0)/lx;
	rec.v = (z-z0)/lz;

	rec.t = t;
#ifndef NDEBUG
	if (!std::isfinite(rec.t)) std::cout << "xz_rec collision gave infinite time" << std::endl;
#endif


	return true;
}

void xz_rect::hit_info(const ray&r, const double t_min, const double t_max, hit_record& rec) {
    const auto outward_normal = vec3(0, 1, 0);	//the trivial normal vector
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(rec.t);
}


bool yz_rect::hit_time(const ray&r, const double t_min, const double t_max, hit_record& rec) {
	const auto t = (k-r.origin().x()) / r.direction().x();	//time of collision - see aabb.h for why this is the case

	if (t < t_min || t > t_max)	//if collision is outside of specified times
		return false;
	
	//the x and y components of the ray at the time of collision
	const auto z = r.origin().z() + t*r.direction().z();
	const auto y = r.origin().y() + t*r.direction().y();

	//the collision algorithm
	if (z < z0 || z > z1 || y < y0 || y > y1)
		return false;	//there was no collision
	
	//normalise y and z to be used as texture coords
	rec.v = (z-z0)/lz;
	rec.u = (y-y0)/ly;

	rec.t = t;
#ifndef NDEBUG
	if (!std::isfinite(rec.t)) std::cout << "yz_rec collision gave infinite time" << std::endl;
#endif



	return true;
}

void yz_rect::hit_info(const ray&r, const double t_min, const double t_max, hit_record& rec) {
    const auto outward_normal = vec3(1, 0, 0);	//the trivial normal vector
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(rec.t);
}
