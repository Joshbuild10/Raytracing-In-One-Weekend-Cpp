#include "../../../source/Raytracing-The-Next-Week/utility.h"
#include "../../../source/Raytracing-The-Next-Week/colour.h"
#include "../../../source/Raytracing-The-Next-Week/sphere.h"
#include "../../../source/Raytracing-The-Next-Week/hittable_list.h"
#include "../../../source/Raytracing-The-Next-Week/camera.h"
#include "../../../source/Raytracing-The-Next-Week/material.h"
#include "../../../source/Raytracing-The-Next-Week/moving_sphere.h"
#include "../../../source/Raytracing-The-Next-Week/bvh.h"
#include "../../../source/Raytracing-The-Next-Week/aarect.h"
#include "../../../source/Raytracing-The-Next-Week/box.h"
#include "../../../source/Raytracing-The-Next-Week/constant_medium.h"

#include <iostream>

hittable_list cornell_smoke()
{
    hittable_list objects;
    auto red = make_shared<lambertian>(colour(.65, .05, .05));
    auto white = make_shared<lambertian>(colour(.73, .73, .73));
    auto green = make_shared<lambertian>(colour(.12, .45, .15));
    auto light = make_shared<diffuse_light>(colour(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(box1, 0.01, colour(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, colour(1, 1, 1)));

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
    const auto aspect_ratio = 1.0;
    const int image_width = 600;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    colour background = colour(0.0, 0.0, 0.0);

    // World
    hittable_list world = cornell_smoke();

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    point3 lookfrom = point3(278, 278, -800);
    point3 lookat = point3(278, 278, 0);
    auto vfov = 40.0;

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