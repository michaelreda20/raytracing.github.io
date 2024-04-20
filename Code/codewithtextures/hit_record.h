// Inside hit_record.h file:

#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "vec3.h"
#include "ray.h"
#include "color.h"

class HitRecord {
public:
    float t;
    Vec3 point;
    Vec3 normal;
    Color color;

   HitRecord() : normal(Vec3(0, 0, 0)) {}
    // Function to set the face normal based on the ray direction
    void set_face_normal(const Ray& ray, const Vec3& outward_normal) {
        if (Vec3::dot(ray.direction, outward_normal) < 0) {
            normal = outward_normal;
        } else {
            normal = -outward_normal;
        }
    }
};

#endif // HIT_RECORD_H
