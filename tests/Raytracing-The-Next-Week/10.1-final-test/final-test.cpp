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

hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(colour(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(colour(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<lambertian>(colour(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(colour(0.8, 0.8, 0.9), 1.0)
        ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, colour(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, colour(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(colour(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) 
    {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
        vec3(-100, 270, 395)
        )
    );
    return objects;
}

colour ray_colour(const ray& r, const colour& background, const hittable& world, int depth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return colour(0, 0, 0);

    hit_record rec;
    // If the ray hits nothing, return the background colour.
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
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    colour background = colour(0.0, 0.0, 0.0);

    // World
    hittable_list world = final_scene();

    // Camera

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    point3 lookfrom = point3(478, 278, -600);
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