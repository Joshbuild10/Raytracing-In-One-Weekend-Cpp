#include "../../../source/Raytracing-In-One-Weekend/utility.h"
#include "../../../source/Raytracing-In-One-Weekend/colour.h"
#include "../../../source/Raytracing-In-One-Weekend/sphere.h"
#include "../../../source/Raytracing-In-One-Weekend/hittable_list.h"
#include "../../../source/Raytracing-In-One-Weekend/camera.h"
#include "../../../source/Raytracing-In-One-Weekend/material.h"

#include <iostream>

colour ray_colour(const ray& r, const hittable& world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return colour(0, 0, 0);

    hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) // Returns the scaled normal of the nearest object (if any) hit
    {
        ray scattered;
        colour attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_colour(scattered, world, depth - 1);
        return colour(0, 0, 0);
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
    auto R = cos(pi / 4);
    hittable_list world;

    auto material_ground = make_shared<lambertian>(colour(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(colour(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(colour(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // Camera

    point3 lookfrom(3, 3, 2);
    point3 lookat(0, 0, -1);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    auto aperture = 2.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

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