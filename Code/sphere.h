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
    //setcenter
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
