// Assume this is your Camera class definition and implementation in "camera.h"

#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"
#include "image_writer.h" 
#include "hittablelist.h" 
#include "color.h" // Include Color class
#include "hit_record.h" // Include HitRecord class
#include <nlohmann/json.hpp>
#include <fstream> // Include fstream for file operations

using namespace std;
using json = nlohmann::json;

class Camera {
public:
    int width;
    int height;
    Vec3 position;
    Vec3 lookAt;
    Vec3 upVector;
    float fov;
    float exposure;

    // Constructor to initialize camera properties from JSON
    Camera(const nlohmann::json& cameraJson)
        : width(cameraJson["width"]),
          height(cameraJson["height"]),
          position(Vec3(cameraJson["position"])),
          lookAt(Vec3(cameraJson["lookAt"])),
          upVector(Vec3(cameraJson["upVector"])),
          fov(cameraJson["fov"]),
          exposure(cameraJson["exposure"]) {}

    // Function to render the scene
    void render(const HittableList& world);

private:
    // Add any additional private members as needed

    // Function to generate a ray based on pixel coordinates
    inline Ray generateRay(float u, float v) const {
        // Compute the direction of the ray based on pixel coordinates
        Vec3 direction = (lookAt - position).normalized();

        // Adjust the direction based on the pixel coordinates, FOV, etc.
        // For simplicity, we'll use a basic pinhole camera model.
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        float halfWidth = tan(0.5 * fov);
        float halfHeight = halfWidth / aspectRatio;

        Vec3 horizontal = 2.0 * halfWidth * (upVector.cross(direction)).normalized();
        Vec3 vertical = 2.0 * halfHeight * upVector.normalized();

        // Compute the final direction of the ray
        direction = (direction + u * horizontal + v * vertical).normalized();

        return Ray(position, direction);
    }
};

// Inline implementation of the render function
void Camera::render(const HittableList& world) {
    int width = this->width;
    int height = this->height;

    // Open the file in binary mode
    ofstream outfile("output.ppm", ios::binary);
    if (!outfile.is_open()) {
        cerr << "Error opening output file." << endl;
        return;
    }

    // PPM header
    outfile << "P6\n" << width << " " << height << "\n255\n";

    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(width);
            float v = static_cast<float>(j) / static_cast<float>(height);

            Ray ray = generateRay(u, v);

            HitRecord rec;

            // Check if the ray hits any object in the world
            if (world.hit(ray, 0.0, std::numeric_limits<float>::infinity(), rec)) {
                // Use the hit record to determine the color
                Color pixelColor = 0.5f * (Color(rec.normal.x(), rec.normal.y(), rec.normal.z()) + Color(1.0f, 1.0f, 1.0f));
                pixelColor.clamp(); // Ensure color values are within the valid range

                // Write the color to the output file in binary mode
                outfile.put(static_cast<char>(pixelColor.r * 255));
                outfile.put(static_cast<char>(pixelColor.g * 255));
                outfile.put(static_cast<char>(pixelColor.b * 255));
            } else {
                // If the ray doesn't hit any object, set the background color
                Vec3 unit_direction = ray.direction.normalized();
                float t = 0.5f * (unit_direction.y + 1.0f);
                Color bgColor = (1.0f - t) * Color(1.0f, 1.0f, 1.0f) + t * Color(0.5f, 0.7f, 1.0f);
                bgColor.clamp(); // Ensure color values are within the valid range

                // Write the background color to the output file in binary mode
                outfile.put(static_cast<char>(bgColor.r * 255));
                outfile.put(static_cast<char>(bgColor.g * 255));
                outfile.put(static_cast<char>(bgColor.b * 255));
            }
        }
    }

    // Close the output file
    outfile.close();
}

#endif // CAMERA_H
