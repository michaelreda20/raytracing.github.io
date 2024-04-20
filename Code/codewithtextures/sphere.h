/*
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"  // Include the header file for Vec3
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Sphere : public hittable {
public:
    Vec3 center;
    float radius;
 

    Sphere(const nlohmann::json& sphereJson): hittable(),
          center(Vec3(sphereJson["center"][0], sphereJson["center"][1], sphereJson["center"][2])),
          radius(sphereJson["radius"]){}

    bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override {
        Vec3 oc = ray.origin - center;
        float a = Vec3::dot(ray.direction, ray.direction);
        float half_b = Vec3::dot(oc, ray.direction);
        float c = oc.length_squared() - radius * radius;

        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;

        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.point = ray.at(rec.t);
        Vec3 outward_normal = (rec.point - center) / radius;
        rec.set_face_normal(ray, outward_normal);
        

        return true;
    }
};

#endif // SPHERE_H
*/

#ifndef SPHERE_H
#define SPHERE_H

#include "Ray.h"
#include "Vec3.h"
#include <nlohmann/json.hpp>
#include "material.h"

class Sphere {
public:
    Vec3 center;
    float radius;

    //Sphere(const Vec3& center, float radius) : center(center), radius(radius) {}

    Sphere(const nlohmann::json& json, const Material& material) : Sphere(json) {
        this->material = material;
    }

   Sphere(const nlohmann::json& json) {
    if (json.find("center") != json.end() && json["center"].is_array() && json["center"].size() == 3) {
        center = Vec3(json["center"][0], json["center"][1], json["center"][2]);
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'center' key in Sphere JSON");
    }

    if (json.find("radius") != json.end()) {
        radius = json["radius"];
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'radius' key in Sphere JSON");
    }

}
    Vec3 normalAt(const Vec3& point) const {
        // Calculate the normal vector at the given point on the sphere
        return (point - center).normalized();
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
        float a = ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y + ray.direction.z * ray.direction.z;
        float b = 2.0f * (oc.x * ray.direction.x + oc.y * ray.direction.y + oc.z * ray.direction.z);
        float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            //cout << "discriminant " << discriminant << endl;
            return false;
        }
      //  cout<<"discriminant "<<discriminant<<endl;

        t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        return true;
    }
private:
    Material material;
};

#endif // SPHERE_H
