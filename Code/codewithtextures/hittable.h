// Inside shape.h file:

#ifndef HITTABLE_H
#define HITTABLE_H

#include "hit_record.h"

#include "ray.h"
#include "color.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
// Forward declare the HitRecord class
class HitRecord;

class hittable {
public:
    virtual bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const = 0;
};

// Include the full definition of the HitRecord class here

#endif // SHAPE_H
