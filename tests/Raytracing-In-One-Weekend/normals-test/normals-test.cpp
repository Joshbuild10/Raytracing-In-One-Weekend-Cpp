#include "../../../source/Raytracing-In-One-Weekend/vec3.h"
#include "../../../source/Raytracing-In-One-Weekend/colour.h"
#include "../../../source/Raytracing-In-One-Weekend/ray.h"

#include <iostream>

// Checks if ray collided with a sphere
double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;

    // If point not on sphere return -1, else return solution of the quadratic
    return (discriminant < 0) ? -1.0 : (-half_b - sqrt(discriminant)) / a;
}

colour ray_colour(const ray& r)
{
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    if (t > 0.0) // If point on sphere
    {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1)); // Computes surface normal
        return 0.5 * (N + colour(1, 1, 1)); // Scales normal to colour range
    }

    vec3 unit_direction = unit_vector(r.direction()); // Normalises the vector
    t = 0.5 * (unit_direction.y + 1.0); // Scales to range 0 <= t <= 1
    return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0); // Linear interpolation algorithm
}

int main()
{

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera

    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush; // Progress indicator
        for (int i = 0; i < image_width; ++i)
        {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            colour pixel_colour = ray_colour(r);
            write_colour(std::cout, pixel_colour); // Writes colour to output buffer
        }
    }
    std::cerr << "\nDone.\n";
}