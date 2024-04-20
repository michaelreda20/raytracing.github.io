/*
// PinholeCamera.h
#ifndef PINHOLE_CAMERA_H
#define PINHOLE_CAMERA_H

#include "Ray.h"
#include "Vec3.h"

class PinholeCamera {
public:
    Vec3 position;
    Vec3 lookAt;
    Vec3 upVector;
    float fov; // Field of view in degrees
    double exposure;

    PinholeCamera(const Vec3& position, const Vec3& lookAt, const Vec3& upVector, float fov, double exposure)
        : position(position), lookAt(lookAt), upVector(upVector), fov(fov), exposure(exposure) {}

    Ray generateRay(float u, float v) const {
        Vec3 direction = (lookAt - position).normalized();
        Vec3 horizontal = upVector.cross(direction).normalized();
        Vec3 vertical = direction.cross(horizontal).normalized();

        // Calculate the point on the image plane
        Vec3 pointOnImagePlane = position + horizontal * u + vertical * v + direction;

        // Calculate the direction of the ray
        Vec3 rayDirection = (pointOnImagePlane - position).normalized();

        // Return the generated ray
        return Ray(position, rayDirection);
    }
};

#endif // PINHOLE_CAMERA_H
*/


// PinholeCamera.h
#ifndef PINHOLE_CAMERA_H
#define PINHOLE_CAMERA_H

#include "Ray.h"
#include "Vec3.h"

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

    PinholeCamera(const Vec3& position, const Vec3& lookAt, const Vec3& upVector, float fov, double exposure, int width, int height)
        : position(position), lookAt(lookAt), upVector(upVector), fov(fov), exposure(exposure), width(width), height(height) {
        initialize();
    }

    

    void initialize() {
        // Calculate camera basis
        // Vec3 direction = (lookAt - position).normalized();
        // horizontal = upVector.cross(direction).normalized();
        // vertical = direction.cross(horizontal).normalized();

        // float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        // float halfHeight = tan((fov * M_PI / 180.0) / 2.0);
        // float halfWidth = aspectRatio * halfHeight;

        // // Adjust the top-left corner based on exposure
        // top_left_corner = position + halfWidth * horizontal + halfHeight * vertical;
        // top_left_corner = top_left_corner - (exposure - 1) * direction;
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

    Ray generateRay(float u, float v) const {
        // Calculate the point on the image plane
        // Vec3 pointOnImagePlane = top_left_corner - u * horizontal - v * vertical;

        // // Calculate the direction of the ray with exposure adjustment
        // Vec3 rayDirection = (pointOnImagePlane - position).normalized() * exposure;
           Vec3 rayDirection = imageCenter + (2.0 * u - 1.0) * halfWidth * right +
                        (1.0 - 2.0 * v) * halfHeight * upward - position;

        // Return the generated ray
        return Ray(position, rayDirection);
    }
};

#endif // PINHOLE_CAMERA_H
