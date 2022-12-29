#include "../../../source/Raytracing-The-Next-Week/utility.h"
#include "../../../source/Raytracing-The-Next-Week/colour.h"
#include "../../../source/Raytracing-The-Next-Week/sphere.h"
#include "../../../source/Raytracing-The-Next-Week/hittable_list.h"
#include "../../../source/Raytracing-The-Next-Week/camera.h"
#include "../../../source/Raytracing-The-Next-Week/material.h"
#include "../../../source/Raytracing-The-Next-Week/moving_sphere.h"
#include "../../../source/Raytracing-The-Next-Week/bvh.h"

#include <iostream>

hittable_list two_perlin_spheres()
{
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

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
    const int samples_per_pixel = 50;
    const int max_depth = 50;

    // World
    hittable_list world = two_perlin_spheres();

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    point3 lookfrom = point3(13, 2, 3);
    point3 lookat = point3(0, 0, 0);
    auto vfov = 20.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

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