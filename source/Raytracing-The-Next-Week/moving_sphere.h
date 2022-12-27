#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "utility.h"

#include "hittable.h"


class moving_sphere : public hittable {
public:
    point3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<material> mat_ptr;


    moving_sphere() {}
    moving_sphere(
        point3 cen0,
        point3 cen1, 
        double _time0, 
        double _time1, 
        double r, 
        shared_ptr<material> m)
        : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r), mat_ptr(m)
    {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    point3 center(double time) const;
};

point3 moving_sphere::center(double time) const { return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0); }

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    vec3 oc = r.origin() - center(r.time());
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - radius * radius;
    double discriminant = half_b * half_b - a * c;

    if (discriminant < 0) return false;
    double sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    double root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root)
    {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}
#endif