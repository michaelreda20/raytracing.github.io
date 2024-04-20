// PinholeCamera.h
#ifndef PINHOLE_CAMERA_H
#define PINHOLE_CAMERA_H

#include "Ray.h"
#include "Vec3.h"
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class PinholeCamera {
public:
    Vec3 position;
    Vec3 lookAt;
    Vec3 upVector;
    float fov; // Field of view in degrees
    double exposure;
    int width;
    int height;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 top_left_corner;
    Vec3 forward;
    Vec3 right;
    Vec3 upward;
    Vec3 imageCenter;
    float halfWidth;
    float halfHeight;
    float aperture;




    PinholeCamera(const Vec3& position, const Vec3& lookAt, const Vec3& upVector, float fov, double exposure, int width, int height, float aperture = 0.1f)
        : position(position), lookAt(lookAt), upVector(upVector), fov(fov), exposure(exposure), width(width), height(height), aperture(aperture) {
        initialize();
    }

    void initialize() {
        // Calculate the camera coordinate system
        forward = (lookAt - position).normalized();
        right = forward.cross(upVector).normalized();
        upward = right.cross(forward).normalized();

        // Calculate the image plane center
        imageCenter = position + forward;

        // Calculate halfWidth and halfHeight
        float aspectRatio = static_cast<float>(width) / height;
        halfHeight = tan(fov * 0.5);
        halfWidth = aspectRatio * halfHeight;
    }
    //random float function
    float random_float() const {
        static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
        static std::mt19937 generator;
        return distribution(generator);
    }
Ray generateRay(float u, float v) const {
        // Generate a random point on the aperture
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> distribution(-aperture / 2.0f, aperture / 2.0f);
        float lensU = distribution(generator);
        float lensV = distribution(generator);

        // Calculate ray direction with lens sampling
        Vec3 lensPoint = position + getLensRadius() * (lensU * right + lensV * upward);
        Vec3 rayDirection = imageCenter + (2.0 * u - 1.0) * halfWidth * right +
                            (1.0 - 2.0 * v) * halfHeight * upward - lensPoint;

        // Return the generated ray
        return Ray(lensPoint, rayDirection.normalized());
    }



    
    // Get the lens radius
   
   
   
    float getLensRadius() const {
        return aperture / 2.0f;
    }

    // Get the focus distance
    float getFocusDistance() const {
        return (lookAt - position).length();
    }


    // Get the lens center
       Vec3 getLensCenter() const {
        return position + getFocusDistance() * forward;
    }
};

#endif // PINHOLE_CAMERA_H
