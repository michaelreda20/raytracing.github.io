#ifndef VIDEO_H
#define VIDEO_H
#include "Ray.h"
#include "Vec3.h"
#include "pinhole_camera.h"
#include "sphere.h"
#include "cylinder.h"
#include "triangle.h"
#include "point_light.h"
#include "image_writer.h"


// Function to move an object within the scene
void moveObjectWithinScene(std::vector<Sphere>& spheres, const std::vector<Cylinder>& cylinders, const Vec3& offset) {
    // Move the sphere and cylinder to the new position
    spheres[0].setCenter(spheres[0].center + offset);
    cylinders[0].setCenter(cylinders[0].center + offset);
}

// Function to render images with moving objects
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

    // Loop through different positions of the moving objects
    for (int frame = 0; frame < numFrames; ++frame) {
        // Calculate the new position of the moving objects
        float offsetX = static_cast<float>(frame) * 0.1f;  // Adjust the offset based on your scene
        Vec3 offset(offsetX, 0.0f, 0.0f);

        // Move the objects to the new position
        moveObjectWithinScene(spheres, cylinders, offset);

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

        // Reset the positions of the moving objects for the next frame
        spheres[0].setCenter(originalSpherePosition);
        cylinders[0].setCenter(originalCylinderPosition);
    }
}

#endif // VIDEO_H