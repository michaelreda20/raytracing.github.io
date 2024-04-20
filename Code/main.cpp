#include "Ray.h"
#include "Vec3.h"
#include "image_writer.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Triangle.h"
#include "pinhole_camera.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include "point_light.h"
#include "material.h"
#include <sstream>


using json = nlohmann::json;
using namespace std;
string rendermode;
float t;


Vec3 renderPixel(const PinholeCamera& camera, const std::vector<Sphere>& spheres,
                 const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                 const std::vector<PointLight>& lights, int nbounces,
                 float u, float v, int width, int height, const Ray& ray);


Vec3 computeColor(const Ray& ray, const std::vector<Sphere>& spheres,
                  const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                  const std::vector<PointLight>& lights, int nbounces);

Vec3 calculateShading(const Ray& ray, const Vec3& hit_point, const Vec3& normal, const Material& material,
                      const std::vector<PointLight>& lights, int nbounces,
                      const std::vector<Sphere>& spheres, const std::vector<Cylinder>& cylinders,
                      const std::vector<Triangle>& triangles);


// Function to generate a random float between 0 and 1
float random_float() {
    return static_cast<float>(rand()) / RAND_MAX;
}

Vec3 reinhardToneMapping(const Vec3& color, float exposure) {
    float L_w = 0.2126f * color.x + 0.7152f * color.y + 0.0722f * color.z;  // Luminance

    // Tone mapping formula
    Vec3 mappedColor = color * (exposure / (exposure + L_w));

    // Optionally, you can perform gamma correction for display
    float gamma = 2.2f;
    mappedColor.x = std::pow(mappedColor.x, 1.0f / gamma);
    mappedColor.y = std::pow(mappedColor.y, 1.0f / gamma);
    mappedColor.z = std::pow(mappedColor.z, 1.0f / gamma);

    return mappedColor;
}


// Helper function to clamp the y-coordinate of an object's position within a specified range
Vec3 clampPosition(const Vec3& position, float minY, float maxY) {
    Vec3 clampedPosition = position;
    clampedPosition.y = std::max(minY, std::min(maxY, position.y));
    return clampedPosition;
}


void renderImagesWithMovingObjects(const PinholeCamera& camera,
                                   std::vector<Sphere>& spheres,
                                   std::vector<Cylinder>& cylinders,
                                   const std::vector<Triangle>& triangles,
                                   const std::vector<PointLight>& lights,
                                   int nbounces,
                                   int numFrames,
                                   const std::string& outputDirectory) {
    Vec3 originalSpherePosition = spheres[0].center;
    Vec3 originalCylinderPosition = cylinders[0].center;

    // Define motion parameters
    float sphereSpeed = 0.2f;  // Speed of the sphere
    float cylinderSpeed = 0.1f;  // Speed of the cylinder
    float maxYPosition = 1.0f;
    float minYPosition = -0.5f;
    float groundHeight = minYPosition + 0.1f;  // Adjust as needed
    bool isDescending = true;  // Flag to indicate whether the sphere is descending

    // Loop through different positions of the moving objects
    for (int frame = 0; frame < numFrames; ++frame) {
        // Calculate the new position of the sphere
        float offsetY = (isDescending ? -1.0f : 1.0f) * sphereSpeed * std::sin(static_cast<float>(frame) * 0.1f);
        Vec3 offset(0.0f, offsetY, 0.0f);

        // Move the sphere to the new position
        spheres[0].setCenter(originalSpherePosition + offset);

        // Ensure the sphere stays within the camera range and above the ground
        spheres[0].setCenter(clampPosition(spheres[0].center, minYPosition, maxYPosition));

        // Calculate the new position of the cylinder
        float offsetCylinderY = (isDescending ? 1.0f : -1.0f) * cylinderSpeed * std::sin(static_cast<float>(frame) * 0.1f);
        Vec3 offsetCylinder(0.0f, offsetCylinderY, 0.0f);

        // Move the cylinder to the new position
        cylinders[0].setCenter(originalCylinderPosition + offsetCylinder);

        // Ensure the cylinder stays within the camera range and above the ground
        cylinders[0].setCenter(clampPosition(cylinders[0].center, minYPosition, maxYPosition));

        // Check if the sphere is about to reach the ground
        if (isDescending && spheres[0].center.y <= groundHeight) {
            isDescending = false;  // Switch to ascending motion
        }

        // Render the scene
        Vec3* image = new Vec3[camera.width * camera.height];

        for (int j = 0; j < camera.height; ++j) {
            for (int i = 0; i < camera.width; ++i) {
                float u = static_cast<float>(i) / static_cast<float>(camera.width);
                float v = 1.0f - static_cast<float>(j) / static_cast<float>(camera.height);
                Ray ray = camera.generateRay(u, v);

                Vec3 color = renderPixel(camera, spheres, cylinders, triangles, lights, nbounces, u, v, camera.width, camera.height, ray);

                image[j * camera.width + i] = color;
            }
        }

        // Save the image with a filename indicating the frame number
        std::ostringstream filename;
        filename << outputDirectory << "/frame_" << frame << ".ppm";
        ImageWriter::writePPM(filename.str().c_str(), camera.width, camera.height, image);

        // Cleanup
        delete[] image;
    }

    // Reset the positions of the moving objects
    spheres[0].setCenter(originalSpherePosition);
    cylinders[0].setCenter(originalCylinderPosition);
}



Vec3 reflect(const Vec3& incident, const Vec3& normal) {
    return incident - 2.0f * Vec3::dot(incident, normal) * normal;
}

// Function to compute refraction direction (Snell's Law)
Vec3 refract(const Vec3& incident, const Vec3& normal, float refractiveIndexRatio) {
    float cos_theta = std::min(Vec3::dot(-incident, normal), 1.0f);
    Vec3 perpendicular = refractiveIndexRatio * (incident + cos_theta * normal);
    Vec3 parallel = -std::sqrt(std::abs(1.0f - perpendicular.length_squared())) * normal;
    return perpendicular + parallel;
}

bool checkShadow(const Ray& shadow_ray, const std::vector<Sphere>& spheres,
                 const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles) {
    float t;
    for (const auto& sphere : spheres) {
        if (sphere.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
            return true;
        }
    }

    for (const auto& cylinder : cylinders) {
        if (cylinder.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
            return true;
        }
    }

    for (const auto& triangle : triangles) {
        if (triangle.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
            return true;
        }
    }

    return false;
}

Vec3 handleReflection(const Ray& ray, const Vec3& hit_point, const Vec3& normal, const Material& material,
                      const std::vector<PointLight>& lights,
                      int nbounces, const std::vector<Sphere>& spheres,
                      const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles) {
    Vec3 reflection_color(0.0f, 0.0f, 0.0f);

    if (material.isreflective && material.reflectivity > 0.0f) {
        Vec3 reflected_direction = reflect(ray.direction, normal).normalized();
        Ray reflected_ray(hit_point + normal * 0.01f, reflected_direction);  // Increase the offset
        reflection_color = material.reflectivity * computeColor(reflected_ray, spheres, cylinders, triangles, lights, nbounces - 1);
    }

    return reflection_color;
}


Vec3 handleRefraction(const Ray& ray, const Vec3& hit_point, const Vec3& normal, const Material& material,
                      const std::vector<PointLight>& lights,int nbounces, const std::vector<Sphere>& spheres, const std::vector<Cylinder>& cylinders,
                      const std::vector<Triangle>& triangles) {
    Vec3 color(0.0f, 0.0f, 0.0f);

    if (material.isrefractive && material.refractiveindex > 0.0f) {
        Vec3 refracted_direction = refract(ray.direction, normal, 1.0f / material.refractiveindex).normalized();
        Ray refracted_ray(hit_point - normal * 0.001f, refracted_direction);
        color += (1.0f - material.reflectivity) * computeColor(refracted_ray, spheres, cylinders, triangles, lights, nbounces - 1);
    }

    return color;
}


Vec3 computeColor(const Ray& ray, const std::vector<Sphere>& spheres,
                  const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                  const std::vector<PointLight>& lights, int nbounces) {
    if (nbounces <= 0) {
        // End recursion when reaching the maximum number of bounces
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    float t;
    Vec3 color(0.0f, 0.0f, 0.0f);
    Vec3 hit_point, normal;
    float closest_hit = std::numeric_limits<float>::infinity();

    // Iterate over spheres, cylinders, and triangles
    for (const auto& sphere : spheres) {
        if (sphere.intersect(ray, t) && t < closest_hit) {
            closest_hit = t;
            hit_point = ray.origin + t * ray.direction;
            normal = sphere.normalAt(hit_point);
            Material material = sphere.getMaterial();
            color = calculateShading(ray, hit_point, normal, material, lights, nbounces, spheres, cylinders, triangles);
        }
    }

    for (const auto& cylinder : cylinders) {
        if (cylinder.intersect(ray, t) && t < closest_hit) {
            closest_hit = t;
            hit_point = ray.origin + t * ray.direction;
            normal = cylinder.normalAt(hit_point);
            Material material = cylinder.getMaterial();
            color = calculateShading(ray, hit_point, normal, material, lights, nbounces, spheres, cylinders, triangles);
        }
    }

    for (const auto& triangle : triangles) {
        if (triangle.intersect(ray, t) && t < closest_hit) {
            closest_hit = t;
            hit_point = ray.origin + t * ray.direction;
            normal = triangle.normal();
            Material material = triangle.getMaterial();
            color = calculateShading(ray, hit_point, normal, material, lights, nbounces, spheres, cylinders, triangles);
        }
    }

    return color;
}

// ...

Vec3 calculateShading(const Ray& ray, const Vec3& hit_point, const Vec3& normal, const Material& material,
                      const std::vector<PointLight>& lights, int nbounces,
                      const std::vector<Sphere>& spheres, const std::vector<Cylinder>& cylinders,
                      const std::vector<Triangle>& triangles) {
    float ambient_factor = material.ka;
    Vec3 ambient = ambient_factor * material.diffusecolor;
    Vec3 color(0.0f, 0.0f, 0.0f);

    if (rendermode == "binary") {
        color = Vec3(1.0f, 0.0f, 0.0f);  // Red color
    } else if (rendermode == "phong") {
        for (const auto& light : lights) {
            Vec3 light_direction = (light.position - hit_point).normalized();
            Vec3 view_direction = (ray.origin - hit_point).normalized();
            Vec3 halfway = (view_direction + light_direction).normalized();

            // Shadow check
            Ray shadow_ray(hit_point + normal * 0.001f, light_direction);
            bool in_shadow = checkShadow(shadow_ray, spheres, cylinders, triangles);

            if (!in_shadow) {
                float diffuse_intensity = std::max(0.0f, Vec3::dot(normal, light_direction));
                float specular_intensity = std::pow(std::max(0.0f, Vec3::dot(normal, halfway)), material.specularexponent);

                Vec3 diffuse = material.diffusecolor * light.intensity * diffuse_intensity * material.kd;
                Vec3 specular = material.specularcolor * light.intensity * specular_intensity * material.ks;

                color += ambient + diffuse + specular;

                // Light sampling
                constexpr int num_samples = 10;  // Adjust the number of samples as needed
                for (int i = 0; i < num_samples; ++i) {
                    // Sample random point on the light source
                    Vec3 light_sample_point = light.position;  // Modify this line to sample randomly

                    // Compute direction from hit point to the sampled point on the light
                    Vec3 light_sample_direction = (light_sample_point - hit_point).normalized();

                    // Compute shadow ray to the sampled point on the light
                    Ray shadow_ray_sample(hit_point + normal * 0.001f, light_sample_direction);

                    // Check if the sampled point is visible from the hit point
                    bool in_shadow_sample = checkShadow(shadow_ray_sample, spheres, cylinders, triangles);

                    if (!in_shadow_sample) {
                        // Adjust the intensity based on the distance between the hit point and the sampled point
                        float distance_factor = 1.0f / (light_sample_point - hit_point).length_squared();

                        Vec3 diffuse_sample = material.diffusecolor * light.intensity * diffuse_intensity * material.kd * distance_factor;
                        Vec3 specular_sample = material.specularcolor * light.intensity * specular_intensity * material.ks * distance_factor;

                        color += diffuse_sample + specular_sample;
                    }
                }
            }
        }

        // Handle reflection and refraction (recursive)
        if (nbounces > 0) {
            Vec3 reflection_color = handleReflection(ray, hit_point, normal, material, lights, nbounces, spheres, cylinders, triangles);
            color += reflection_color;
            color += handleRefraction(ray, hit_point, normal, material, lights, nbounces, spheres, cylinders, triangles);
        }
    }

    return color;
}

// ...


// Rest of the functions remain unchanged...


// Function to perform anti-aliased rendering
// Function to perform anti-aliased rendering with lens sampling
Vec3 renderPixel(const PinholeCamera& camera, const std::vector<Sphere>& spheres,
                 const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                 const std::vector<PointLight>& lights, int nbounces,
                 float u, float v, int width, int height, const Ray& ray) {
    const int num_samples = 10;  // You can adjust this value based on your anti-aliasing needs
    Vec3 color = Vec3(0.0f, 0.0f, 0.0f);

    // Generate rays with random offsets within the pixel and on the lens
    for (int i = 0; i < num_samples; ++i) {
        // Lens sampling
        float lens_u = random_float();
        float lens_v = random_float();
        Vec3 lens_point = camera.getLensCenter() + camera.getLensRadius() * (2.0f * Vec3(lens_u, lens_v, 0.0f) - Vec3(1.0f, 1.0f, 0.0f));
        
        // Generate ray from the lens point to the image plane
        Vec3 focal_point = ray.origin + camera.getFocusDistance() * ray.direction;
        Vec3 new_origin = lens_point;
        Vec3 new_direction = (focal_point - lens_point).normalized();
        // Ray new_ray(new_origin, new_direction);

        // Compute color using the new ray
        color += computeColor(ray, spheres, cylinders, triangles, lights, nbounces);
    }

    // Average the colors
    color /= static_cast<float>(num_samples);
    float exposure = 1.0f;  // You can adjust the exposure value
    color = reinhardToneMapping(color, exposure);

    return color;
}

PinholeCamera parseCamera(const json& cameraConfig) {
    Vec3 cameraPosition(cameraConfig["position"][0], cameraConfig["position"][1], cameraConfig["position"][2]);
    Vec3 lookAt(cameraConfig["lookAt"][0], cameraConfig["lookAt"][1], cameraConfig["lookAt"][2]);
    Vec3 upVector(cameraConfig["upVector"][0], cameraConfig["upVector"][1], cameraConfig["upVector"][2]);
    float fov = cameraConfig["fov"];
    double exposure = cameraConfig["exposure"];
    int width = cameraConfig["width"];
    int height = cameraConfig["height"];
    float aperture = 0.1f;  // Default aperture value

    return PinholeCamera(cameraPosition, lookAt, upVector, fov, exposure, width, height,aperture);
}
// set the back ground color to black

Material parseMaterial(const nlohmann::json& shapeConfig) {
    if (shapeConfig.find("material") != shapeConfig.end()) {
        return Material(shapeConfig["material"]);
    } else {
        // If no material is specified, create a default material
        return Material(0.0f, 0.0f,0.0f, 1.0f, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), false, 0.0f, false, 1.0f);
    }
}


void parseShapes(const json& shapesConfig, std::vector<Sphere>& spheres, std::vector<Cylinder>& cylinders, std::vector<Triangle>& triangles) {
    // Use a map to store shapes based on their type
    std::map<std::string, std::vector<json>> shapeMap;

    // Collect all shapes based on their type
    for (const auto& shape : shapesConfig) {
        if (!shape.contains("type")) {
            std::cerr << "Error: Shape does not have a 'type' key.\n";
            continue;
        }

        std::string type = shape["type"];
        shapeMap[type].emplace_back(shape);
    }

    // Process spheres
    for (const auto& sphereConfig : shapeMap["sphere"]) {
        Material sphereMaterial = parseMaterial(sphereConfig); // Parse material for sphere
        Sphere sphere(sphereConfig, sphereMaterial);
        spheres.emplace_back(sphere);
        // print sphere confgi
        // cout<<"sphere config: "<<sphereConfig<<endl;
    }

    // Process cylinders
    for (const auto& cylinderConfig : shapeMap["cylinder"]) {
        Material cylinderMaterial = parseMaterial(cylinderConfig); // Parse material for cylinder
        Cylinder cylinder(cylinderConfig, cylinderMaterial);
        cylinders.emplace_back(cylinder);
        // cout<<"cylinder config: "<<cylinderConfig<<endl;

    }

    // Process triangles
    for (const auto& triangleConfig : shapeMap["triangle"]) {
        Material triangleMaterial = parseMaterial(triangleConfig); // Parse material for triangle
        Triangle triangle(triangleConfig, triangleMaterial);
        triangles.emplace_back(triangle);
        // cout<<"triangle config: "<<triangleConfig<<endl;
    }
}

static std::vector<PointLight> parseLights(const nlohmann::json& sceneConfig) {
        std::vector<PointLight> lights;

        if (sceneConfig.find("lightsources") != sceneConfig.end() && sceneConfig["lightsources"].is_array()) {
            for (const auto& lightConfig : sceneConfig["lightsources"]) {
                if (!lightConfig.contains("type")) {
                    std::cerr << "Error: Light source does not have a 'type' key.\n";
                    continue;
                }

                std::string type = lightConfig["type"];

                if (type == "pointlight") {
                    lights.emplace_back(lightConfig);
                } else {
                    std::cerr << "Error: Unsupported light type: " << type << "\n";
                }
            }
        }

        return lights;
    }

Vec3 parseBackgroundColor(const json& sceneConfig) {
    Vec3 defaultColor = {0.0f, 0.0f, 0.0f}; // Default color if not specified

    if (sceneConfig.find("backgroundcolor") != sceneConfig.end() && sceneConfig["backgroundcolor"].is_array() && sceneConfig["backgroundcolor"].size() == 3) {
        // Extract RGB values from the JSON array
        return {
            sceneConfig["backgroundcolor"][0],
            sceneConfig["backgroundcolor"][1],
            sceneConfig["backgroundcolor"][2]
        };
    } else {
        std::cerr << "Warning: Using default background color.\n";
        return defaultColor;
    }
}



int main() {
    std::ifstream ifs("scene_phong.json");
    if (!ifs.is_open()) {
        std::cerr << "Error opening JSON file\n";
        return 1;
    }

    json config;
    ifs >> config;

    // Add checks to print or log JSON content
    //std::cout << "Config JSON:\n" << config.dump(2) << std::endl;

    const int width = config["camera"]["width"];
    const int height = config["camera"]["height"];

    Vec3* image = new Vec3[width * height];

    // Initialize camera
    PinholeCamera camera = parseCamera(config["camera"]);

    // Create shapes based on the information in the JSON file
    std::vector<Sphere> spheres;
    std::vector<Cylinder> cylinders;
    std::vector<Triangle> triangles;

    // Parse background color
    Vec3 backgroundColor = parseBackgroundColor(config["scene"]);


    parseShapes(config["scene"]["shapes"], spheres, cylinders, triangles);
    std::vector<PointLight> lights = parseLights(config["scene"]);
  int nbounces = config.contains("nbounces") ? config["nbounces"].get<int>() : 1; // Adjust the default value as needed

    cout<<"nbounces: "<<nbounces<<endl;
    rendermode = config["rendermode"];
    cout<<"rendermode: "<<rendermode<<endl;
    


    srand(static_cast<unsigned>(time(0))); // Seed for random number generation


    for (int j = 0; j < height; ++j) {  // Change loop condition to start from the top
        for (int i = 0; i < width; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(width);
            float v = 1.0f - static_cast<float>(j) / static_cast<float>(height);
            Ray ray = camera.generateRay(u, v);
            Vec3 color = renderPixel(camera, spheres, cylinders, triangles, lights, nbounces, u, v, width, height,ray);
            if (rendermode =="phong")
            {
            color+=backgroundColor;
            }
            image[j * width + i] = color;
        }
    }

    ImageWriter::writePPM("output.ppm", width, height, image);

    delete[] image;

    


    const int numFrames = 20;  // Adjust the number of frames based on your requirements
    
    std::string outputDirectory = "output_images";

 // Call the function to render images with a moving object
    //renderImagesWithMovingObjects(camera, spheres, cylinders, triangles, lights, nbounces, numFrames, outputDirectory);
    
    return 0;
}
