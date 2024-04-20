#include "image_writer.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Triangle.h"
#include "pinhole_camera.h"
#include <nlohmann/json.hpp>
#include "point_light.h"
#include <map>
#include <sstream>
#include "stb-master/stb-master/stb_image.h"  // Include the stb_image library
#include <string>
#include <cmath>  // Include the cmath header for trigonometric functions



using json = nlohmann::json;
using namespace std;
string rendermode;
float t;

Vec3 renderPixel(const PinholeCamera& camera, const std::vector<Sphere>& spheres,
                 const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                 const std::vector<PointLight>& lights, int nbounces,
                 float u, float v, int width, int height, const Ray& ray);

// Helper function to get texture color based on material
Vec3 getTextureColor(const Material& material, float u, float v) {
    return material.texture.sample(u, v);
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



// Function to generate a random float between 0 and 1
float random_float() {
    return static_cast<float>(rand()) / RAND_MAX;
}

Vec3 calculateShading(const Vec3& normal) {
    Vec3 lightDirection(1.0f, 1.0f, 1.0f);  // Example light direction
    lightDirection = lightDirection.normalized();
    float intensity = std::max(0.0f, Vec3::dot(normal,lightDirection));
    return Vec3(intensity, 0.0f, 0.0f);  // Red color based on intensity
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


// Function to compute the color by tracing the ray through the scene
Vec3 computeColor(const Ray& ray, const std::vector<Sphere>& spheres,
                  const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                  const std::vector<PointLight>& lights, int nbounces, float u, float v) {
    if (nbounces <= 0) {
        // End recursion when reaching the maximum number of bounces
        return Vec3(0.0f, 0.0f, 0.0f);
    }

    float t;
    Vec3 color(0.0f, 0.0f, 0.0f);
    Vec3 hit_point, normal;
    float closest_hit = std::numeric_limits<float>::infinity();  // Initialize with a large value

    // Iterate over spheres
    for (const auto& sphere : spheres) {
        if (sphere.intersect(ray, t) && t < closest_hit) {
            closest_hit = t;
            hit_point = ray.origin + t * ray.direction;
            normal = sphere.normalAt(hit_point);
            Material material = sphere.getMaterial();
           // Vec3 textureColor = getTextureColor(material, u, v); // Get texture color


            if (rendermode == "binary") {
                color = Vec3(1.0f, 0.0f, 0.0f);  // Red color
            } else if (rendermode == "phong") {
                // Implement Blinn-Phong shading for spheres
                Vec3 ambient = material.ambientcolor * material.diffusecolor; // Ambient term

                for (const auto& light : lights) {
                    Vec3 light_direction = (light.position - hit_point).normalized();
                    Vec3 view_direction = (ray.origin - hit_point).normalized();
                    Vec3 halfway = (view_direction + light_direction).normalized();

                    // Shadow check
                    Ray shadow_ray(hit_point + normal * 0.008f, light_direction);

                    bool in_shadow = false;
                    for (const auto& shadow_object : spheres) {
                        if (shadow_object.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
                            in_shadow = true;
                            break;
                        }
                    }

                    if (!in_shadow) {
                        float diffuse_intensity = std::max(0.0f, Vec3::dot(normal, light_direction));
                        float specular_intensity = std::pow(std::max(0.0f, Vec3::dot(normal, halfway)), material.specularexponent);

                          // Use texture color in shading calculations
                    //  Vec3 diffuse = textureColor * light.intensity * diffuse_intensity * material.kd;
                    //  Vec3 specular = textureColor * light.intensity * specular_intensity * material.ks;
                         Vec3 diffuse = light.intensity * diffuse_intensity * material.kd;
                     Vec3 specular = light.intensity * specular_intensity * material.ks;

                    color += ambient * material.ambientcolor + diffuse + specular;
                    }
                }

                // Handle reflection (recursive)
                if (nbounces > 0 && material.isreflective && material.reflectivity > 0.0f) {
                    Vec3 reflected_direction = reflect(ray.direction, normal);
                    Ray reflected_ray(hit_point + normal * 0.001f, reflected_direction);
                    color += material.reflectivity * computeColor(reflected_ray, spheres, cylinders, triangles, lights, nbounces - 1, u, v);
                }

                // Handle refraction (recursive)
                if (nbounces > 0 && material.isrefractive && material.refractiveindex > 0.0f) {
                    Vec3 refracted_direction = refract(ray.direction, normal, 1.0f / material.refractiveindex);
                    Ray refracted_ray(hit_point - normal * 0.001f, refracted_direction);
                    color += (1.0f - material.reflectivity) * computeColor(refracted_ray, spheres, cylinders, triangles, lights, nbounces - 1, u, v);
                }
            }
        }
    }

    // Iterate over cylinders
    for (const auto& cylinder : cylinders) {
        if (cylinder.intersect(ray, t) && t < closest_hit) {
            closest_hit = t;
            hit_point = ray.origin + t * ray.direction;
            normal = cylinder.normalAt(hit_point);
            Material material = cylinder.getMaterial();
            // Vec3 textureColor = getTextureColor(material, u, v); // Get texture color


            if (rendermode == "binary") {
                color = Vec3(1.0f, 0.0f, 0.0f);  // Red color
            } else if (rendermode == "phong") {
                // Implement Blinn-Phong shading for cylinders
                Vec3 ambient = material.ambientcolor * material.diffusecolor; // Ambient term

                for (const auto& light : lights) {
                    Vec3 light_direction = (light.position - hit_point).normalized();
                    Vec3 view_direction = (ray.origin - hit_point).normalized();
                    Vec3 halfway = (view_direction + light_direction).normalized();

                    // Shadow check
                   Ray shadow_ray(hit_point + normal * 0.008f, light_direction);
                    bool in_shadow = false;
                    for (const auto& shadow_object : spheres) {
                        if (shadow_object.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
                            in_shadow = true;
                            break;
                        }
                    }

                    // Include shadow check for cylinders
                    for (const auto& shadow_object : cylinders) {
                        if (shadow_object.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
                            in_shadow = true;
                            break;
                        }
                    }

                    if (!in_shadow) {
                        float diffuse_intensity = std::max(0.0f, Vec3::dot(normal, light_direction));
                        float specular_intensity = std::pow(std::max(0.0f, Vec3::dot(normal, halfway)), material.specularexponent);
                    // Use texture color in shading calculations
                        // Vec3 diffuse = textureColor * light.intensity * diffuse_intensity * material.kd;
                        // Vec3 specular = textureColor * light.intensity * specular_intensity * material.ks;
                        Vec3 diffuse =  light.intensity * diffuse_intensity * material.kd;
                        Vec3 specular =  light.intensity * specular_intensity * material.ks;
                        color += ambient * material.ambientcolor + diffuse + specular;
                    }
                }

                // Handle reflection (recursive)
                if (nbounces > 0 && material.isreflective && material.reflectivity > 0.0f) {
                    Vec3 reflected_direction = reflect(ray.direction, normal);
                    Ray reflected_ray(hit_point + normal * 0.001f, reflected_direction);
                    color += material.reflectivity * computeColor(reflected_ray, spheres, cylinders, triangles, lights, nbounces - 1, u, v);
                }

                // Handle refraction (recursive)
                if (nbounces > 0 && material.isrefractive && material.refractiveindex > 0.0f) {
                    Vec3 refracted_direction = refract(ray.direction, normal, 1.0f / material.refractiveindex);
                    Ray refracted_ray(hit_point - normal * 0.001f, refracted_direction);
                    color += (1.0f - material.reflectivity) * computeColor(refracted_ray, spheres, cylinders, triangles, lights, nbounces - 1, u, v);
                }
            }
        }
    }

    // Iterate over triangles
    for (const auto& triangle : triangles) {
        if (triangle.intersect(ray, t) && t < closest_hit) {
            closest_hit = t;
            hit_point = ray.origin + t * ray.direction;
            normal = triangle.normal();
            Material material = triangle.getMaterial();
            // Vec3 textureColor = getTextureColor(material, u, v); // Get texture color


            if (rendermode == "binary") {
                color = Vec3(1.0f, 0.0f, 0.0f);  // Red color
            } else if (rendermode == "phong") {
                // Implement Blinn-Phong shading for triangles
                Vec3 ambient = material.ambientcolor * material.diffusecolor; // Ambient term

                for (const auto& light : lights) {
                    Vec3 light_direction = (light.position - hit_point).normalized();
                    Vec3 view_direction = (ray.origin - hit_point).normalized();
                    Vec3 halfway = (view_direction + light_direction).normalized();

                    // Shadow check
                    Ray shadow_ray(hit_point + normal * 0.008f, light_direction);
                    bool in_shadow = false;
                    for (const auto& shadow_object : spheres) {
                        if (shadow_object.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
                            in_shadow = true;
                            break;
                        }
                    }

                    // Include shadow check for cylinders
                    for (const auto& shadow_object : cylinders) {
                        if (shadow_object.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
                            in_shadow = true;
                            break;
                        }
                    }

                    // Include shadow check for triangles
                    for (const auto& shadow_object : triangles) {
                        if (&shadow_object != &triangle && shadow_object.intersect(shadow_ray, t) && t > 0.001f && t < 1.0f) {
                            in_shadow = true;
                            break;
                        }
                    }

                    if (!in_shadow) {
                        float diffuse_intensity = std::max(0.0f, Vec3::dot(normal, light_direction));
                        float specular_intensity = std::pow(std::max(0.0f, Vec3::dot(normal, halfway)), material.specularexponent);

                        // Use texture color in shading calculations
                        // Vec3 diffuse = textureColor * light.intensity * diffuse_intensity * material.kd;
                        // Vec3 specular = textureColor * light.intensity * specular_intensity * material.ks;
                        Vec3 diffuse = light.intensity * diffuse_intensity * material.kd;
                        Vec3 specular = light.intensity * specular_intensity * material.ks;

                        color += ambient * material.ambientcolor + diffuse + specular;
                    }
                }

                // Handle reflection (recursive)
                if (nbounces > 0 && material.isreflective && material.reflectivity > 0.0f) {
                    Vec3 reflected_direction = reflect(ray.direction, normal);
                    Ray reflected_ray(hit_point + normal * 0.001f, reflected_direction);
                    color += material.reflectivity * computeColor(reflected_ray, spheres, cylinders, triangles, lights, nbounces - 1, u, v);
                }

                // Handle refraction (recursive)
                if (nbounces > 0 && material.isrefractive && material.refractiveindex > 0.0f) {
                    Vec3 refracted_direction = refract(ray.direction, normal, 1.0f / material.refractiveindex);
                    Ray refracted_ray(hit_point - normal * 0.001f, refracted_direction);
                    color += (1.0f - material.reflectivity) * computeColor(refracted_ray, spheres, cylinders, triangles, lights, nbounces - 1, u, v);
                }
            }
        }
    }

    return color;
}

// Function to perform anti-aliased rendering
Vec3 renderPixel(const PinholeCamera& camera, const std::vector<Sphere>& spheres,
                 const std::vector<Cylinder>& cylinders, const std::vector<Triangle>& triangles,
                 const std::vector<PointLight>& lights, int nbounces,
                 float u, float v, int width, int height, const Ray& ray) {
    const int num_samples = 10;  // You can adjust this value based on your anti-aliasing needs
    Vec3 color = Vec3(0.0f, 0.0f, 0.0f);

    // Generate rays with random offsets within the pixel
    for (int i = 0; i < num_samples; ++i) {

        //Ray ray = camera.generateRay(new_u, new_v);
        color += computeColor(ray, spheres, cylinders, triangles, lights, nbounces, u, v);
    }

    // Average the colors
    color /= static_cast<float>(num_samples);

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

    return PinholeCamera(cameraPosition, lookAt, upVector, fov, exposure, width, height);
}


Material parseMaterial(const nlohmann::json& shapeConfig) {
    if (shapeConfig.find("material") != shapeConfig.end() && shapeConfig["material"].is_object()) {
        cout << "material" << endl;
        
        ImageTexture texture;

        if (shapeConfig["material"].find("texture") != shapeConfig["material"].end() && shapeConfig["material"]["texture"].is_string()) {
            cout << "material1" << endl;
            texture = ImageTexture(shapeConfig["material"]["texture"]);
            cout << "material2" << endl;
        } else {
            // Set a default texture or handle the absence of the texture key
            // For now, I'm setting it to a default texture with an empty string
            cout << "material3" << endl;
            texture = ImageTexture();
        }

        // Use the new Material constructor with the parsed JSON and texture
        cout << "material4" << endl;
        Material material(shapeConfig["material"], texture);

        return material;
    } else {
        // If no material is specified or if it's not an object, create a default material
        cout << "material5" << endl;
        return Material();
    }
}




void parseShapes(const json& shapesConfig, std::vector<Sphere>& spheres, std::vector<Cylinder>& cylinders, std::vector<Triangle>& triangles) {
    // Use a map to store shapes based on their type
    std::map<std::string, std::vector<json>> shapeMap;
cout<<"here"<<endl;
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
        cout<<"here1"<<endl;
        Material sphereMaterial = parseMaterial(sphereConfig); // Parse material for sphere
        Sphere sphere(sphereConfig, sphereMaterial);
        spheres.emplace_back(sphere);
    }

    // Process cylinders
    for (const auto& cylinderConfig : shapeMap["cylinder"]) {
        Material cylinderMaterial = parseMaterial(cylinderConfig); // Parse material for cylinder
        Cylinder cylinder(cylinderConfig, cylinderMaterial);
        cylinders.emplace_back(cylinder);
        cout<<"here2"<<endl;
    }

    // Process triangles
    for (const auto& triangleConfig : shapeMap["triangle"]) {
        Material triangleMaterial = parseMaterial(triangleConfig); // Parse material for triangle
        Triangle triangle(triangleConfig, triangleMaterial);
        triangles.emplace_back(triangle);
        cout<<"here3"<<endl;
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
    std::ifstream ifs("simple_phong.json");
    if (!ifs.is_open()) {
        std::cerr << "Error opening JSON file\n";
        return 1;
    }

    json config;
    ifs >> config;

    // Add checks to print or log JSON content
    std::cout << "Config JSON:\n" << config.dump(2) << std::endl;

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


    std::vector<PointLight> lights = parseLights(config["scene"]);
    int nbounces = config["nbounces"];
    cout<<"nbounces: "<<nbounces<<endl;
    rendermode = config["rendermode"];
    cout<<"rendermode: "<<rendermode<<endl;

    
    
    parseShapes(config["scene"]["shapes"], spheres, cylinders, triangles);


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
