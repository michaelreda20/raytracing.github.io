// point_light.h file:

#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Vec3.h"
#include <nlohmann/json.hpp>

class PointLight {
public:
    Vec3 position;
    Vec3 intensity;

    PointLight(const Vec3& position, const Vec3& intensity)
        : position(position), intensity(intensity) {}

    PointLight(const nlohmann::json& json) {
        if (json.find("position") != json.end() && json["position"].is_array() && json["position"].size() == 3) {
            position = Vec3(json["position"][0], json["position"][1], json["position"][2]);
        } else {
            throw std::invalid_argument("Invalid or missing 'position' key in PointLight JSON");
        }

        if (json.find("intensity") != json.end() && json["intensity"].is_array() && json["intensity"].size() == 3) {
            intensity = Vec3(json["intensity"][0], json["intensity"][1], json["intensity"][2]);
        } else {
            throw std::invalid_argument("Invalid or missing 'intensity' key in PointLight JSON");
        }
    }
};

#endif // POINT_LIGHT_H
