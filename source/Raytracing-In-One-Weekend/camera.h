#ifndef CAMERA_H
#define CAMERA_H

#include "utility.h"

class camera
{
private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;

public:
    camera( // Default parameters for back-compatibility with tests
        point3 lookfrom = vec3(0, 0, 0),
        point3 lookat = vec3(0, 0, -1),
        vec3 vup = vec3(0, 1, 0), // Camera roll
        double vfov = 90, // Vertical field of view (in degrees)
        double aspect_ratio = 16.0 / 9.0,
        double aperture = 0,
        double focus_dist = 1) 
	{
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
    }

    ray get_ray(double s, double t) const
	{
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x + v * rd.y;

        return ray(origin + offset,lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }
};
#endif