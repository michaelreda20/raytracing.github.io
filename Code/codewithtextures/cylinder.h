/*
#ifndef CYLINDER_H
#define CYLINDER_H

#include "Hittable.h"
#include <nlohmann/json.hpp>
#include "vec3.h"
#include "color.h"
using json = nlohmann::json;
class Cylinder : public hittable {
public:
    Vec3 center;
    Vec3 axis;
    float radius;
    float height;
    
    Cylinder(const Vec3& center, const Vec3& axis, float radius, float height, const Color& color)
        : center(center), axis(axis), radius(radius), height(height) {}

    // Constructor to initialize from a JSON object
    Cylinder(const nlohmann::json& j);

    bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;
};

inline Cylinder::Cylinder(const nlohmann::json& j) {
    // Assuming the JSON structure is as follows
    center = Vec3(j["center"][0], j["center"][1], j["center"][2]);
    axis = Vec3(j["axis"][0], j["axis"][1], j["axis"][2]);
    radius = j["radius"];
    height = j["height"];
    
}

bool Cylinder::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
    Vec3 oc = ray.origin - center;
    float a = Vec3::dot(ray.direction - Vec3::dot(ray.direction, axis) * axis, ray.direction - Vec3::dot(ray.direction, axis) * axis);
    float b = 2.0 * Vec3::dot(oc - Vec3::dot(oc, axis) * axis, ray.direction - Vec3::dot(ray.direction, axis) * axis);
    float c = Vec3::dot(oc - Vec3::dot(oc, axis) * axis, oc - Vec3::dot(oc, axis) * axis) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        float root = (-b - sqrt(discriminant)) / (2.0 * a);
        if (root < t_max && root > t_min) {
            rec.t = root;
            rec.point = ray.at(rec.t);
            Vec3 outward_normal = (rec.point - center - Vec3::dot(rec.point - center, axis) * axis) / radius;
            rec.set_face_normal(ray, outward_normal);
           
            return true;
        }

        root = (-b + sqrt(discriminant)) / (2.0 * a);
        if (root < t_max && root > t_min) {
            rec.t = root;
            rec.point = ray.at(rec.t);
            Vec3 outward_normal = (rec.point - center - Vec3::dot(rec.point - center, axis) * axis) / radius;
            rec.set_face_normal(ray, outward_normal);
         
            return true;
        }
    }

    return false;
}


#endif // CYLINDER_H
*/

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
// set the center
void setCenter(const Vec3& center) {
    this->center = center;
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
