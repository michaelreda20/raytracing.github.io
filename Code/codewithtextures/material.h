// material.h file:

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vec3.h"
#include <nlohmann/json.hpp>
#include "imagetexture.h"

class Material {
public:
    float ks;               // Specular reflection coefficient
    float kd;               // Diffuse reflection coefficient
    float specularexponent; // Specular reflection exponent
    Vec3 diffusecolor;      // Diffuse color
    Vec3 specularcolor;     // Specular color
    Vec3 ambientcolor;      // Ambient color
    bool isreflective;      // Reflective property
    float reflectivity;     // Reflectivity coefficient
    bool isrefractive;      // Refractive property
    float refractiveindex;  // Refractive index
    ImageTexture texture;

    Material(
        float ks, float kd, float specularexponent,
        const Vec3& diffusecolor, const Vec3& specularcolor,
        const Vec3& ambientcolor,
        bool isreflective, float reflectivity,
        bool isrefractive, float refractiveindex,
    const ImageTexture& texture) : ks(ks), kd(kd), specularexponent(specularexponent),
        diffusecolor(diffusecolor), specularcolor(specularcolor),
        ambientcolor(ambientcolor),
        isreflective(isreflective), reflectivity(reflectivity),
        isrefractive(isrefractive), refractiveindex(refractiveindex), texture(texture) {}
    //default constructor
    Material() : ks(0.0f), kd(0.0f), specularexponent(1.0f),
        diffusecolor(Vec3(0.0f, 0.0f, 0.0f)), specularcolor(Vec3(0.0f, 0.0f, 0.0f)),
        ambientcolor(Vec3(0.4f, 0.4f, 0.4f)),
        isreflective(false), reflectivity(0.0f),
        isrefractive(false), refractiveindex(1.0f) {}
     Material(const nlohmann::json& json, const ImageTexture& texture) : Material(json) {
        this->texture = texture;
      }

    // settexture
    void setTexture(const ImageTexture& texture) {
        this->texture = texture;
    }

    Material(const nlohmann::json& json) {
        ks = json.value("ks", 0.0f);
        kd = json.value("kd", 0.0f);
        specularexponent = json.value("specularexponent", 1.0f);

        if (json.find("diffusecolor") != json.end() && json["diffusecolor"].is_array() && json["diffusecolor"].size() == 3) {
            diffusecolor = Vec3(json["diffusecolor"][0], json["diffusecolor"][1], json["diffusecolor"][2]);
        } else {
            // Handle error or throw an exception
            throw std::invalid_argument("Invalid or missing 'diffusecolor' key in Material JSON");
        }

        if (json.find("specularcolor") != json.end() && json["specularcolor"].is_array() && json["specularcolor"].size() == 3) {
            specularcolor = Vec3(json["specularcolor"][0], json["specularcolor"][1], json["specularcolor"][2]);
        } else {
            // Handle error or throw an exception
            throw std::invalid_argument("Invalid or missing 'specularcolor' key in Material JSON");
        }
        if (json.find("texture") != json.end()) {
             setTexture(ImageTexture(json));
        }
        ambientcolor= Vec3(0.4f, 0.4f, 0.4f);

        isreflective = json.value("isreflective", false);
        reflectivity = json.value("reflectivity", 0.0f);
        isrefractive = json.value("isrefractive", false);
        refractiveindex = json.value("refractiveindex", 1.0f);
    }

        void updateFromJson(const nlohmann::json& materialJson) {
        // Check if each key exists in the JSON object before updating
       

        if (materialJson.find("diffusecolor") != materialJson.end()) {
            diffusecolor = Vec3(materialJson["diffusecolor"][0], materialJson["diffusecolor"][1], materialJson["diffusecolor"][2]);
        }
        if(materialJson.find("texture") != materialJson.end()){
            texture = ImageTexture(materialJson["texture"]);
        }
        if (materialJson.find("specularcolor") != materialJson.end()) {
            specularcolor = Vec3(materialJson["specularcolor"][0], materialJson["specularcolor"][1], materialJson["specularcolor"][2]);
        }
      
        if (materialJson.find("ks") != materialJson.end()) {
            ks = materialJson["ks"];
        }
        if (materialJson.find("kd") != materialJson.end()) {
            kd = materialJson["kd"];
        }
        if (materialJson.find("specularexponent") != materialJson.end()) {
            specularexponent = materialJson["specularexponent"];
        }
        if (materialJson.find("isreflective") != materialJson.end()) {
            isreflective = materialJson["isreflective"];
        }
        if (materialJson.find("reflectivity") != materialJson.end()) {
            reflectivity = materialJson["reflectivity"];
        }
        if (materialJson.find("isrefractive") != materialJson.end()) {
            isrefractive = materialJson["isrefractive"];
        }
        if (materialJson.find("refractiveindex") != materialJson.end()) {
            refractiveindex = materialJson["refractiveindex"];
        }


    }


    
};

#endif // MATERIAL_H
