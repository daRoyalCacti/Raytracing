#include <iostream>
#include "vec3.h"
#include "color.h"
#include "ray.h"


double hit_sphere(const point3& center, const double radius, const ray& r) {
	//using the quadratic equatio to find if 'ray' collides with sphere centred at 'center' with radius 'radius'
	const vec3 oc = r.origin() - center;
	const auto a = r.direction().length_squared();
	const auto b = 2.0 * dot(oc, r.direction());
	const auto c = oc.length_squared() - radius * radius;
	const auto discriminant = b*b - 4*a*c;
	if (discriminant < 0) {
		return -1.0;
	} else {
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
}

color ray_color(const ray& r) {
	//collision with sphere
	auto t = hit_sphere(point3(0,0,-1), 0.5, r);
	if (t > 0.0) {
		vec3 N = unit_vector(r.at(t) - vec3(0,0,-1)); //normal vector; just point on sphere minus center
		return 0.5*color(N.x() + 1, N.y()+1, N.z()+1);
	}
	//background color
	vec3 unit_direction = unit_vector(r.direction());
	t = 0.5*(unit_direction.y() + 1.0);
	return (1.0-t)*color(1.0,1.0,1.0) + t*color(0.5,0.7, 1.0);
}

int main() {
	//image
	const auto aspect_ratio = 16.0 / 9.0;
	const unsigned image_width = 400;
	const unsigned image_height = static_cast<int>(image_width / aspect_ratio);

	//Camera 
	const auto viewport_height = 2.0;
	const auto viewport_width = aspect_ratio * viewport_height;
	const auto focal_length = 1.0;

	const auto origin = point3(0,0,0);
	const auto horizontal = vec3(viewport_width,0,0);
	const auto vertical = vec3(0, viewport_height, 0);
	const auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0,0,focal_length);

	//Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height-1; j>=0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (unsigned i = 0; i < image_width; ++i) {
			auto u = double(i) / (image_width-1);
			auto v = double(j) / (image_height-1);
			ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
			auto pixel_color = ray_color(r);
			write_color(std::cout, pixel_color);
		}
	}

	std::cerr << "\nDone.\n";
}

