// hittable_list_combined.h

#ifndef HITTABLELIST_H
#define HITTABLELIST_H

#include <vector>
#include "hittable.h"
#include "hit_record.h"

class HittableList : public hittable {
public:
    HittableList() = default;

    void add(std::shared_ptr<hittable> hittable) {
        hittables.push_back(hittable);
    }

    virtual bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;

private:
    std::vector<std::shared_ptr<hittable>> hittables;
};

bool HittableList::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for (const auto& hittable : hittables) {
        if (hittable->hit(ray, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

#endif 
