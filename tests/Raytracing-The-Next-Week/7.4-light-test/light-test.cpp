#include "../../../source/Raytracing-The-Next-Week/utility.h"
#include "../../../source/Raytracing-The-Next-Week/colour.h"
#include "../../../source/Raytracing-The-Next-Week/sphere.h"
#include "../../../source/Raytracing-The-Next-Week/hittable_list.h"
#include "../../../source/Raytracing-The-Next-Week/camera.h"
#include "../../../source/Raytracing-The-Next-Week/material.h"
#include "../../../source/Raytracing-The-Next-Week/moving_sphere.h"
#include "../../../source/Raytracing-The-Next-Week/bvh.h"
#include "../../../source/Raytracing-The-Next-Week/aarect.h"

#include <iostream>

hittable_list simple_light()
{
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(colour(4, 4, 4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
    objects.add(make_shared<sphere>(point3(0, 8, 0), 2, difflight));

    return objects;
}

colour ray_colour(const ray& r, const colour& background, const hittable& world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return colour(0, 0, 0);

    hit_record rec;
    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec)) { return background; }

    ray scattered;
    colour attenuation;
    colour emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered)) { return emitted; }

    return emitted + attenuation * ray_colour(scattered, background, world, depth - 1);
}

int main()
{

    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    colour background = colour(0.0, 0.0, 0.0);

    // World
    hittable_list world = simple_light();

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    point3 lookfrom = point3(26, 3, 6);
    point3 lookat = point3(0, 2, 0);
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
                pixel_colour += ray_colour(r, background, world, max_depth);
            }
            write_colour(std::cout, pixel_colour, samples_per_pixel); // Writes colour to output buffer
        }
    }
    std::cerr << "\nDone.\n";
}