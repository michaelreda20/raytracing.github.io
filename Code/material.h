// material.h file:

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vec3.h"
#include <nlohmann/json.hpp>

class Material {
public:
    float ks;               // Specular reflection coefficient
    float kd;               // Diffuse reflection coefficient
    float ka;               // Ambient reflection coefficient
    float specularexponent; // Specular reflection exponent
    Vec3 diffusecolor;      // Diffuse color
    Vec3 specularcolor;     // Specular color
    bool isreflective;      // Reflective property
    float reflectivity;     // Reflectivity coefficient
    bool isrefractive;      // Refractive property
    float refractiveindex;  // Refractive index

    Material(
        float ks, float kd, float ka, float specularexponent,
        const Vec3& diffusecolor, const Vec3& specularcolor,
        bool isreflective, float reflectivity,
        bool isrefractive, float refractiveindex
    ) : ks(ks), kd(kd), ka(ka), specularexponent(specularexponent),
        diffusecolor(diffusecolor), specularcolor(specularcolor),
        isreflective(isreflective), reflectivity(reflectivity),
        isrefractive(isrefractive), refractiveindex(refractiveindex) {}

    // Default constructor with reasonable default values
    Material() : ks(0.0f), kd(0.8f), ka(0.2f), specularexponent(1.0f),
        diffusecolor(Vec3(0.8f, 0.8f, 0.8f)), specularcolor(Vec3(1.0f, 1.0f, 1.0f)),
        isreflective(false), reflectivity(0.0f),
        isrefractive(false), refractiveindex(1.0f) {}

    Material(const nlohmann::json& json) {
        ks = json.value("ks", 0.0f);
        kd = json.value("kd", 0.8f); // Default value
       // ka = json.value("ka", 0.2f); // Default value
       ka =0.2f;
        specularexponent = json.value("specularexponent", 1.0f);

        if (json.find("diffusecolor") != json.end() && json["diffusecolor"].is_array() && json["diffusecolor"].size() == 3) {
            diffusecolor = Vec3(json["diffusecolor"][0], json["diffusecolor"][1], json["diffusecolor"][2]);
        } else {
            throw std::invalid_argument("Invalid or missing 'diffusecolor' key in Material JSON");
        }

        if (json.find("specularcolor") != json.end() && json["specularcolor"].is_array() && json["specularcolor"].size() == 3) {
            specularcolor = Vec3(json["specularcolor"][0], json["specularcolor"][1], json["specularcolor"][2]);
        } else {
            throw std::invalid_argument("Invalid or missing 'specularcolor' key in Material JSON");
        }

        isreflective = json.value("isreflective", false);
        reflectivity = json.value("reflectivity", 0.0f);
        isrefractive = json.value("isrefractive", false);
        refractiveindex = json.value("refractiveindex", 1.0f);
    }
};

#endif // MATERIAL_H
