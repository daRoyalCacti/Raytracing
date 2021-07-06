//Axis-align bounding box class
//are parallelepipeds that are aligned to the axes that bound objects in the scene
//used to speed of light ray intersections

#include "ray.hpp"

#pragma once

struct aabb {
	point3 minimum, maximum;	//the points that define the bounding box
					// in 1D    ->         |        |
					//        origin       min      max
					//       of ray
					//can't be const

	aabb() = default;
	aabb(const point3& a, const point3& b) : minimum(a), maximum(b) {
	    //enforcing that minimnum is indeed the minimum and maximum is indeed the maximum
	    for (size_t i = 0; i < 3; i++) {
            if (minimum[i] > maximum[i]) {
                std::swap(minimum[i], maximum[i]);
            }
        }
	}

	inline double surface_area() const {
	    const double length = maximum.x() - minimum.x();
	    const double width = maximum.y() - minimum.y();
	    const double height = maximum.z() - minimum.z();

	    return 2*length*width + 2*width*height + 2*length*height;
	}

	[[nodiscard]] point3 min() const {return minimum;}
	[[nodiscard]] point3 max() const {return maximum;}

	[[nodiscard]] inline bool hit(const ray& r, double t_min, double t_max) const {
		//Andrew Kensler (from Pixar) intersection method
		for (int i = 0; i < 3; i++) {
			const double invD = 1.0 / r.direction()[i];	// 1/x or 1/y or 1/z for the incoming ray
			auto t0 = (min()[i] - r.origin()[i]) * invD;	//the time it takes the ray to hit the 'min' side of bounding box
									// s=d/t => t = d/s
									// d is the distance from origin to bounding box in a single direction
									// - so d = (min.x - origin.x)   (see picture above)
									// the ray is defined as r = r0 + dir*t
									// this can be taken as r = r0 + vel * time assuming the speed is 1 (norm of velocity)
									// therefore, t = (min.x - origin.x) / dir.x

			auto t1 = (max()[i] - r.origin()[i]) * invD;	//same as for t0 but for the 'max' side of the bounding box

			if (invD < 0.0) {std::swap(t0, t1);}	//enforcing that t0 < t1
			                                            //this not the immediately the case if the ray is moving towards the origin
			                                            // - see the picture above
			                                            //really only saves 1 if statement and makes the code neater

			//enforcing that the ray hit times are between [t_min, t_max]
			// - tmax and tmin get changed for each component (i.e. first for x, then for y, then for z)
			// - this is a way of determining which side of the box the ray enters and exits at
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;

			if (t_max <= t_min)		//as stated above tmax and tmin get changed for each component (i.e. first for x, then for y, then for z)
				return false;		//the only way for this to return true is if the time taken for a ray to collided with
							// - max in say the y direction is smaller than the time taken for the ray to collide with min in the x direction
							// - min in say the y direction is bigger than the time taken for the ray to collide with max in the x direction
							//in both cases there is no time for which the ray is inside the box
							// - the ray collides with max in 1 dimension then min in the other dimension
							//   -- once it collides with max, it is outside the box
							//   -- i.e. the ray is outside the box in 1D, inside (by colliding with min) in 1D, outside (by colliding with max),
							//      then inside in another dimension by colliding with min
							// - still the ray collides with max in one dimension then the other
							//   -- colliding with min in the y direction after colliding with max in the x direction means
							//      the ray collided with max in the x direction first then min in the y direction
							//if by the end t_max is less than t_min, the ray is inside the box for t in [t_min, t_max]
							// - the time for with the ray is after min but before max in all of x,y,z
							//
							//consider a 2D box and 3 possible rays (time increasing moving right -- collide with min first then max)
							// - requires tab size of 8
							//		      min.x			      max.x
							//
							//		    	|			      /	| tmax.x
							//		tmin.x /|			     /	|
							//		      /	|			    /	|
							//	             /	|			   /	|
							//	            /	|			  /	|
							//	           /	|			 /	|
							//	          /	|			/	|		 /
							//	         /	|		       /	|		/
							//	        /	|		      /		|       tmax.y /
							//-----------------------------------------------------------------------     max.y
							//    tmax.y  /		|	   tmax.y   /		|            /
							//           /		|		   /		|           /
							//          /		|		  /		|          /
							//         /		|		 /		|         /
							//        /		|		/		|        /
							//       /		|	       /		|       /
							//      /		|	      /			|      /
							//     /		|	     /			|     /
							//    /	tmin.y		|   tmin.y  /			|    /
							//-----------------------------------------------------------------------      min.y
							//  /			|	  /			|  / tmin.y
							// /			|	 /		        | /
							//			|	/		        |/
							//			|      /		      	|
							//			|     /		      tmax.x   /|
							//			|    /			      /	|
							//			|   /			     / 	|
							//			|  /			    /	|
							//			| /	tmin.x		   / 	|
							//
							//left ray does not collide because collides with max in the y direction before min in the x direction
							// - tmax.y < tmin.x
							//right ray does not collide because collides with min in the y direction after max in the x direction
							// - tmin.y > tmax.x
							//middle ray is fine
			}
		return true;
	}

};

//creates a larger bounding box around 2 smaller bounding boxes
aabb inline surrounding_box(const aabb &box0, const aabb &box1) {
	const point3 small(	fmin(box0.min().x(),  box1.min().x()),
				fmin(box0.min().y(),  box1.min().y()),
				fmin(box0.min().z(),  box1.min().z()) );

	const point3 big(	fmax(box0.max().x(),  box1.max().x()),
				fmax(box0.max().y(),  box1.max().y()),
				fmax(box0.max().z(),  box1.max().z()) );

	return aabb(small, big);

}
