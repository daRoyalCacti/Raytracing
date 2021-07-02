#pragma once

#include <algorithm>
#include <numeric>

class perlin {
	static constexpr unsigned point_count = 256;
	std::array<vec3, point_count> ranvec;		//array to hold the noise
				    //was original a double array, now vec3 to help with smoothing the noise
	const std::array<int, point_count> perm_x;		//permutations for the x-direction
	const std::array<int, point_count> perm_y;
	const std::array<int, point_count> perm_z;

	static std::array<int, point_count> perlin_generate_perm() {	//creates a permutation of the numbers from 0 to point_count
		//auto p = new int[point_count];
        std::array<int, point_count> p;

		//for (int i = 0; i < point_count; i++)
		//	p[i] = i;
		std::iota(&p[0], &p[point_count], 0);

		permute(p, point_count);

		return p;
	}

	static void permute(std::array<int, point_count> p, const int n) {	//creates a permutation of an integer array of length n
		for (int i = n-1; i>0; i--) {	//switches p[i] with some p[n] for n<i
			const int target = random_int(0,i);
			std::swap(p[i], p[target]);
		}
	}


	static double perlin_interp(const vec3 c[2][2][2], const double u, const double v, const double w) {
		//first using a Hermite cubic to smooth the results
		const auto uu = u*u*(3-2*u);
		const auto vv = v*v*(3-2*v);
		const auto ww = w*w*(3-2*w);

		auto accum = 0.0;

		for (int i=0; i<2; i++)
			for (int j=0; j<2; j++)
				for (int k=0; k<2; k++) {
					const vec3 weight_v(u-i, v-j, w-k);
					accum +=(i*uu + (1-i)*(1-uu)) *	//regal trilinear interpolation
						(j*vv + (1-j)*(1-vv)) *
						(k*ww + (1-k)*(1-ww)) *
						dot(c[i][j][k], weight_v);	//with a slight modification

				}

		return accum;
	}


	public:
	perlin() : perm_x(perlin_generate_perm()), perm_y(perlin_generate_perm()), perm_z(perlin_generate_perm()){
		//ranvec = new vec3[point_count];
		for (int i = 0; i < point_count; i++) {
			ranvec[i] = unit_vector(random_vec3(-1,1));
		}
	}

	~perlin() = default;

	[[nodiscard]] double noise(const point3& p) const {
		//scrambling (using a hash) the random numbers (all point_count of them) to remove tiling
		auto u = p.x() - floor(p.x());	//the decimal part of p.x
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());

		const auto i = static_cast<int>(floor(p.x()));	//used in the scrambling
		const auto j = static_cast<int>(floor(p.y()));
		const auto k = static_cast<int>(floor(p.z()));
		
		vec3 c[2][2][2];


		//smoothing out the result using linear interpolation
		for (int di=0; di<2; di++) {
			for (int dj=0; dj<2; dj++) {
				for (int dk=0; dk<2; dk++) {
					c[di][dj][dk] = ranvec[	//the scrambling for the current point and the a points 1 and 2 integer steps in each direction
						perm_x[(i+di)&255] ^	// - other points are required for the linear interpolation
						perm_y[(j+dj)&255] ^
						perm_z[(k+dk)&255]
					];
				}
			}
		}

		return perlin_interp(c, u, v, w);	//the actual linear interpolation
	}

	[[nodiscard]] double turb(const point3& p, const int depth=7) const {
		auto accum = 0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++) {
			accum += weight * noise(temp_p);	//the actual noise
			weight *= 0.5;				//progressive additions of noise have less impact overall
			temp_p *= 2;				//so the noise is not all at the same place
		}

		return fabs(accum);
	}
};
