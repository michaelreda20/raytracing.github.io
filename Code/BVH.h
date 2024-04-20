// BVH.h
#ifndef BVH_H
#define BVH_H

#include <vector>
#include "Sphere.h"
#include "AABB.h"
#include "hit_record.h"

class BVHNode {
public:
    AABB box;
    BVHNode* left;
    BVHNode* right;
    Sphere* sphere;

    BVHNode();
    BVHNode(const std::vector<Sphere*>& spheres, size_t start, size_t end);

    bool intersect(const Ray& ray, float& t) const;
};

class BVH {
public:
    BVH(const std::vector<Sphere*>& spheres);
    bool intersect(const Ray& ray, float& t) const;

private:
    BVHNode* root;
};

BVHNode::BVHNode() : left(nullptr), right(nullptr), sphere(nullptr) {}

BVHNode::BVHNode(const std::vector<Sphere*>& spheres, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        if (i == start) {
            box = spheres[i]->getBoundingBox();
        } else {
            box = AABB::surroundingBox(box, spheres[i]->getBoundingBox());
        }
    }

    if (end - start > 1) {
        size_t mid = (start + end) / 2;
        left = new BVHNode(spheres, start, mid);
        right = new BVHNode(spheres, mid, end);
    } else {
        sphere = spheres[start];
    }
}

bool BVHNode::intersect(const Ray& ray, HitRecord& hitRecord) const {
    if (!box.intersect(ray)) {
        return false;
    }

    bool hitLeft = left != nullptr ? left->intersect(ray, hitRecord) : false;
    bool hitRight = right != nullptr ? right->intersect(ray, hitRecord) : false;

    return hitLeft || hitRight;
}


BVH::BVH(const std::vector<Sphere*>& spheres) {
    root = new BVHNode(spheres, 0, spheres.size());
}

bool BVH::intersect(const Ray& ray, float& t) const {
    return root->intersect(ray, t);
}

#endif // BVH_H
