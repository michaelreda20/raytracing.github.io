#ifndef CYLINDER_H
#define CYLINDER_H

#include "Ray.h"
#include "Vec3.h"
#include <nlohmann/json.hpp>
#include "material.h"

class Cylinder {
public:
    Vec3 center;
    Vec3 axis;  // Unit vector indicating the direction along which the cylinder extends
    float radius;
    float height;

    Cylinder(const Vec3& center, const Vec3& axis, float radius, float height)
        : center(center), axis(axis.normalized()), radius(radius), height(height) {}

    Cylinder(const nlohmann::json& json, const Material& material) : Cylinder(json) {
        this->material = material;
    }


 Cylinder(const nlohmann::json& j) {
    if (j.find("center") != j.end() && j["center"].is_array() && j["center"].size() == 3) {
        center = Vec3(j["center"][0], j["center"][1], j["center"][2]);
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'center' key in Cylinder JSON");
    }

    if (j.find("axis") != j.end() && j["axis"].is_array() && j["axis"].size() == 3) {
        axis = Vec3(j["axis"][0], j["axis"][1], j["axis"][2]);
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'axis' key in Cylinder JSON");
    }

    if (j.find("radius") != j.end()) {
        radius = j["radius"];
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'radius' key in Cylinder JSON");
    }

    if (j.find("height") != j.end()) {
        height = j["height"];
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'height' key in Cylinder JSON");
    }
    // double the height and move the center 
    height = height * 2;
    center = center - axis * height / 2;

}
Vec3 normalAt(const Vec3& point) const {
    // Calculate the normal vector for a point on the surface of the cylinder
    Vec3 hitPointOnAxis = center + (Vec3::dot(point - center, axis) / Vec3::dot(axis, axis)) * axis;
    Vec3 normal = (point - hitPointOnAxis).normalized();
    
    return normal;
}
// get the material
Material getMaterial() const {
    return material;
}


// set the center
void setCenter(const Vec3& center) {
    this->center = center;
}




    bool intersect(const Ray& ray, float& t) const {
        Vec3 oc = ray.origin - center;

        Vec3 directionPerpendicular = ray.direction - Vec3::dot(ray.direction, axis) * axis;
        float a = Vec3::dot(directionPerpendicular, directionPerpendicular);
        float b = 2.0f * (Vec3::dot(oc,ray.direction) - Vec3::dot(oc,axis) * Vec3::dot(ray.direction,axis));
        float c = Vec3::dot(oc,oc) -  Vec3::dot(oc,axis) *  Vec3::dot(oc,axis) - radius * radius;

        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false;
        }



        float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        // Check if the intersection is within the height of the cylinder
        float intersectionAlongAxis1 = Vec3::dot((ray.origin + t1 * ray.direction - center),axis);
        float intersectionAlongAxis2 = Vec3::dot((ray.origin + t2 * ray.direction - center),axis);

        if ((intersectionAlongAxis1 >= 0) && (intersectionAlongAxis1 <= height)) {
            t = t1;
            return true;

        }

        if ((intersectionAlongAxis2 >= 0) && (intersectionAlongAxis2 <= height)) {
            t = t2;
            return true;

        }

        return false;
    }

    private:
        Material material;
};

#endif // CYLINDER_H
