#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Ray.h"
#include "Vec3.h"
#include <nlohmann/json.hpp>
#include "material.h"
class Triangle {
public:
    Vec3 v0, v1, v2;

    Triangle(const Vec3& v1, const Vec3& v2, const Vec3& v3)
        : v0(v1), v1(v2), v2(v3) {}

    Triangle(const nlohmann::json& json, const Material& material) : Triangle(json) {
        this->material = material;
    }

  Triangle(const nlohmann::json& j) {
    if (j.find("v0") != j.end() && j["v0"].is_array() && j["v0"].size() == 3) {
        v0 = Vec3(j["v0"][0], j["v0"][1], j["v0"][2]);
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'v0' key in Triangle JSON");
    }

    if (j.find("v1") != j.end() && j["v1"].is_array() && j["v1"].size() == 3) {
        v1 = Vec3(j["v1"][0], j["v1"][1], j["v1"][2]);
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'v1' key in Triangle JSON");
    }

    if (j.find("v2") != j.end() && j["v2"].is_array() && j["v2"].size() == 3) {
        v2 = Vec3(j["v2"][0], j["v2"][1], j["v2"][2]);
    } else {
        // Handle error or throw an exception
        throw std::invalid_argument("Invalid or missing 'v2' key in Triangle JSON");
    }
}

Vec3 normal() const {
    // Calculate the normal vector of the triangle
    return (v1-v0).cross(v2 - v0).normalized();
}
// get the material
Material getMaterial() const {
    return material;
}



    bool intersect(const Ray& ray, float& t) const {
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        Vec3 h = ray.direction.cross(e2);
        float a = Vec3::dot(e1,h);

        if (a > -0.00001 && a < 0.00001) {
            return false; // The ray is parallel to the triangle
        }

        float f = 1.0f / a;
        Vec3 s = ray.origin - v0;
        float u = f * Vec3::dot(s,h);

        if (u < 0.0 || u > 1.0) {
            return false;
        }

        Vec3 q = s.cross(e1);
        float v = f * Vec3::dot(ray.direction,q);

        if (v < 0.0 || u + v > 1.0) {
            return false;
        }

        t = f * Vec3::dot(e2,q);

        return t > 0.00001;
    }

    private:
    Material material;
};

#endif // TRIANGLE_H


/*



// triangle.h
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"  // Ensure that the file name is in lowercase
#include "color.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
const float EPSILON = 1e-6;  // Define EPSILON as a small positive value

class Triangle : public hittable {
public:
    Vec3 v0, v1, v2;
   

    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2)
        : v0(v0), v1(v1), v2(v2) {}

    // Constructor to initialize from a JSON object
    Triangle(const nlohmann::json& j);

    bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;
};

inline Triangle::Triangle(const nlohmann::json& j) {
    // Assuming the JSON structure is an array of vertices v0, v1, v2, and a color
    if (j.find("v0") != j.end() && j.find("v1") != j.end() && j.find("v2") != j.end() && j.find("color") != j.end()) {
        v0 = Vec3(j["v0"][0], j["v0"][1], j["v0"][2]);
        v1 = Vec3(j["v1"][0], j["v1"][1], j["v1"][2]);
        v2 = Vec3(j["v2"][0], j["v2"][1], j["v2"][2]);
    } else {
        // Handle the case where the required keys are not present in the JSON
        // For simplicity, set default values or leave the members uninitialized
        v0 = Vec3();  // Default constructor creates a Vec3 with (0, 0, 0)
        v1 = Vec3();
        v2 = Vec3();
      
        // Alternatively, you could throw an exception or take other appropriate actions
        // For example:
        // throw std::invalid_argument("Invalid JSON structure for Triangle. Missing required keys.");
    }
}

// triangle.h
// ...

// triangle.h
// ...

bool Triangle::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const {
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 h = ray.direction.cross(e2);  // Use instance of Vec3 for cross
    float a = Vec3::dot(e1, h);
               // Use instance of Vec3 for dot

    const float EPSILON = 1e-6;

    if (a > -EPSILON && a < EPSILON) {
        return false; // The ray is parallel to the triangle
    }

    float f = 1.0 / a;
    Vec3 s = ray.origin - v0;
    float u = f * Vec3::dot(s, h);
  // Use instance of Vec3 for dot

    if (u < 0.0 || u > 1.0) {
        return false;
    }

    Vec3 q = s.cross(e1);     // Use instance of Vec3 for cross
    float v = f * Vec3::dot(ray.direction, q);
// Use instance of Vec3 for dot

    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    float t = f * Vec3::dot(e2, q);// Use instance of Vec3 for dot

    if (t < t_min || t > t_max) {
        return false;
    }

    rec.t = t;
    rec.point = ray.at(t);
    rec.normal = (e1.cross(e2)).normalized();  // Use instance of Vec3 for cross and normalize

    // Assuming you have a member function named 'set_face_normal' in your HitRecord class
    rec.set_face_normal(ray, rec.normal);

   

    return true;
}

// ...


// ...


#endif // TRIANGLE_H
*/