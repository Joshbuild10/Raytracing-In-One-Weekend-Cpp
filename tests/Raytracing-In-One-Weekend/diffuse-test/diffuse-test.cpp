#include "../../../source/Raytracing-In-One-Weekend/utility.h"
#include "../../../source/Raytracing-In-One-Weekend/colour.h"
#include "../../../source/Raytracing-In-One-Weekend/sphere.h"
#include "../../../source/Raytracing-In-One-Weekend/hittable_list.h"
#include "../../../source/Raytracing-In-One-Weekend/camera.h"

#include <iostream>

colour ray_colour(const ray& r, const hittable& world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return colour(0, 0, 0);

    hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) // Returns the scaled normal of the nearest object (if any) hit
    {
        point3 target = rec.p + rec.normal + random_unit_vector();
        return 0.5 * ray_colour(ray(rec.p, target - rec.p), world, --depth);
    }
    vec3 unit_direction = unit_vector(r.direction()); // Normalises the vector
    auto t = 0.5 * (unit_direction.y + 1.0); // Scales to range 0 <= t <= 1
    return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0); // Linear interpolation algorithm
}

int main()
{

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera

    camera cam;

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush; // Progress indicator
        for (int i = 0; i < image_width; ++i)
        {
            colour pixel_colour(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) // Samples each pixel multiple times and sums the values
            {
                // Generates a random x and y offset
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                ray r = cam.get_ray(u, v);
                pixel_colour += ray_colour(r, world, max_depth);
            }
            write_colour(std::cout, pixel_colour, samples_per_pixel); // Writes colour to output buffer
        }
    }
    std::cerr << "\nDone.\n";
}