#pragma once

#include <utility>

#include "hittable.h"
#include "common.h"

struct triangle : public hittable {
	std::shared_ptr<material> mp;
	vec3 vertex0, vertex1, vertex2;	//position of vertex
	double u_0{}, v_0{}, u_1{}, v_1{}, u_2{}, v_2{};	//texture coords for each vertex
	vec3 v0, v1;	//edges of the triangle
	double d00{}, d01{}, d11{}, invDenom{};	//helpful quantities for finding texture coords

	bool vertex_normals{};	//whether to use face normals or vertex normals

	vec3 normal0, normal1, normal2;	//vertex normals (do not have to be set) 
	
	 triangle() = default;
	 triangle(const vec3 vec0, const vec3 vec1, const vec3 vec2, const double u0_, const double v0_, const double u1_, const double v1_, const double u2_, const double v2_,  std::shared_ptr<material> mat)
		: vertex0(vec0), vertex1(vec1), vertex2(vec2), u_0(u0_), v_0(v0_), u_1(u1_), v_1(v1_), u_2(u2_), v_2(v2_),  mp(std::move(mat)) {

		vertex_normals = false;
		//precomputing some quantities to find the uv coordinates
		//https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates	
		v0 = vertex1 - vertex0;
		v1 = vertex2 - vertex0;

		d00 = dot(v0, v0);
		d01 = dot(v0, v1);
		d11 = dot(v1, v1);
		invDenom = 1.0f / (d00 * d11 - d01 * d01);
		};

	 triangle(const vec3 vec0, const vec3 vec1, const vec3 vec2, const vec3 n0, const vec3 n1, const vec3 n2, const double u0_, const double v0_, const double u1_, const double v1_, const double u2_, const double v2_,  std::shared_ptr<material> mat)
		: triangle(vec0, vec1, vec2, u0_, v0_, u1_, v1_, u2_, v2_, std::move(mat)) {
		normal0 = n0;
		normal1 = n1;
		normal2 = n2;

		vertex_normals = true;
		};

	
	 bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	 bool bounding_box(const double time0, const double time1, aabb& output_box) const override {
		//finding the min and max of each coordinate
		double min_x = vertex0.x(), min_y = vertex0.y(), min_z = vertex0.z();
		double max_x = vertex0.x(), max_y = vertex0.y(), max_z = vertex0.z();

		if (vertex1.x() < min_x)
			min_x = vertex1.x();
		if (vertex1.y() < min_y)
			min_y = vertex1.y();
		if (vertex1.z() < min_z)
			min_z = vertex1.z();

		if (vertex2.x() < min_x)
			min_x = vertex2.x();
		if (vertex2.y() < min_y)
			min_y = vertex2.y();
		if (vertex2.z() < min_z)
			min_z = vertex2.z();


		if (vertex1.x() > max_x)
			max_x = vertex1.x();
		if (vertex1.y() > max_y)
			max_y = vertex1.y();
		if (vertex1.z() > max_z)
			max_z = vertex1.z();

		if (vertex2.x() > max_x)
			max_x = vertex2.x();
		if (vertex2.y() > max_y)
			max_y = vertex2.y();
		if (vertex2.z() > max_z)
			max_z = vertex2.z();

		const double small = 0.0001;
		const double epsilon = 0.000001;
		if (std::abs(min_x - max_x) < epsilon) {
			max_x += small;
			min_x -= small;
		}
		if (std::abs(min_y - max_y) < epsilon) {
			max_y += small;
			min_y -= small;
		}
		if (std::abs(min_z - max_z) < epsilon) {
			max_z += small;
			min_z -= small;
		}

		//creating the bounding box
		output_box = aabb(vec3(min_x, min_y, min_z), vec3(max_x, max_y, max_z) );
		return true;
	}


	 inline void barycentric_coords(const vec3 p, double& Bary0, double& Bary1, double &Bary2) const {
		//find the uv coordinates by interpolating using barycentric coordinates
		//https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates	
		const vec3 v2 = p - vertex0;
		const double d20 = dot(v2, v0);
		const double d21 = dot(v2, v1);

		Bary0 = (d11 * d20 - d01 * d21) * invDenom;
		Bary1 = (d00 * d21 - d01 * d20) * invDenom;
		Bary2 = 1.0f - Bary0 - Bary1;
	}

	 static inline void barycentric_interp(double &out, const double interp0, const double interp1, const double interp2, const double Bary0, const double Bary1, const double Bary2) {
		//https://computergraphics.stackexchange.com/questions/1866/how-to-map-square-texture-to-triangle
		out = Bary2*interp0 + Bary0*interp1 + Bary1*interp2; 
	}

};

 bool triangle::hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const {
	//using the Moller-Trumbore intersection algorithm
	//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	
	const double epsilon = 0.0000001;
	vec3 h, s, q;
	double a, f, u, v;
	
	
	h = cross(r.dir, v1);
        a = dot(v0, h);

	if (a > -epsilon && a < epsilon)	//ray is parallel to triangle
		return false;

	f = 1.0f / a;
	s = r.orig - vertex0;
	u = f * dot(s, h);

	if (u < 0.0f || u > 1.0f)
		return false;

	q = cross(s, v0);
	v = f * dot(r.dir, q);

	if (v < 0.0f | u + v > 1.0f)
		return false;

	//computing the time of intersection
	const double t = f * dot(v1, q);

	if (t < t_min || t > t_max || t < epsilon)	//time of intersection falls outside of time range considered
		return false;

	rec.t = t;

	rec.mat_ptr = mp;
	rec.p = r.at(t);

	//finding the uv coords using interpolation with barycentric coordinates
	double Bary0, Bary1, Bary2;
	barycentric_coords(rec.p, Bary0, Bary1, Bary2);
	barycentric_interp(rec.u, u_0, u_1, u_2, Bary0, Bary1, Bary2);
	barycentric_interp(rec.v, v_0, v_1, v_2, Bary0, Bary1, Bary2);
	
	if (!vertex_normals) {
		rec.set_face_normal(r, cross(v1, v0) );
	} else {
		//interpolating the normal vectors
		vec3 temp_norm_res;
		for (int i = 0; i < 3; i++) {	//for the 3 components of the normal vector
			barycentric_interp(temp_norm_res[i], normal0[i], normal1[i], normal2[i], Bary0, Bary1, Bary2);
		}
		rec.set_face_normal(r, temp_norm_res);
	}


	return true;	
}
