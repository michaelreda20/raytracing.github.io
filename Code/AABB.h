// AABB.h
#ifndef AABB_H
#define AABB_H

#include "Ray.h"

class AABB {
public:
    Vec3 min;
    Vec3 max;

    AABB() : min(Vec3()), max(Vec3()) {}
    AABB(const Vec3& a, const Vec3& b) : min(a), max(b) {}

    static AABB surroundingBox(const AABB& box1, const AABB& box2);

    bool intersect(const Ray& ray, float tMin, float tMax) const;
};

AABB AABB::surroundingBox(const AABB& box1, const AABB& box2) {
    Vec3 small(fmin(box1.min.x, box2.min.x), fmin(box1.min.y, box2.min.y), fmin(box1.min.z, box2.min.z));
    Vec3 big(fmax(box1.max.x, box2.max.x), fmax(box1.max.y, box2.max.y), fmax(box1.max.z, box2.max.z));
    return AABB(small, big);
}

bool AABB::intersect(const Ray& ray, float tMin, float tMax) const {
    float invDirX = 1.0f / ray.direction.x;
    float t0 = (min.x - ray.origin.x) * invDirX;
    float t1 = (max.x - ray.origin.x) * invDirX;

    if (invDirX < 0.0f) std::swap(t0, t1);

    tMin = (t0 > tMin) ? t0 : tMin;
    tMax = (t1 < tMax) ? t1 : tMax;

    if (tMax <= tMin) return false;

    float invDirY = 1.0f / ray.direction.y;
    t0 = (min.y - ray.origin.y) * invDirY;
    t1 = (max.y - ray.origin.y) * invDirY;

    if (invDirY < 0.0f) std::swap(t0, t1);

    tMin = (t0 > tMin) ? t0 : tMin;
    tMax = (t1 < tMax) ? t1 : tMax;

    if (tMax <= tMin) return false;

    float invDirZ = 1.0f / ray.direction.z;
    t0 = (min.z - ray.origin.z) * invDirZ;
    t1 = (max.z - ray.origin.z) * invDirZ;

    if (invDirZ < 0.0f) std::swap(t0, t1);

    tMin = (t0 > tMin) ? t0 : tMin;
    tMax = (t1 < tMax) ? t1 : tMax;

    return tMax > tMin;
}

#endif // AABB_H
