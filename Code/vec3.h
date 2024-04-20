#pragma once

#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
//#include <nlohmann/json.hpp> // Add this include statement


class Vec3 {
public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
   // Vec3(const nlohmann::json& j) : x(j[0]), y(j[1]), z(j[2]) {}


    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    // multiply a vector by a vector
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }


    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 normalized() const {
        float length = std::sqrt(x * x + y * y + z * z);
        return Vec3(x / length, y / length, z / length);
    }
  

    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    // Add this member function to the Vec3 class in vec3.h
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    // print the vector
    void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
    }

 float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    
    // // reflect the vector
    // Vec3 reflect(const Vec3& normal) const {
    //     return *this - 2 * Vec3::dot(*this, normal) * normal;
    // }
    // // refract the vector
    // Vec3 refract(const Vec3& normal, float eta) const {
    //     float cosTheta = std::min(Vec3::dot(-*this, normal), 1.0f);
    //     Vec3 rOutPerpendicular = eta * (*this + cosTheta * normal);
    //     Vec3 rOutParallel = -std::sqrt(std::fabs(1.0f - rOutPerpendicular.length_squared())) * normal;
    //     return rOutPerpendicular + rOutParallel;
    // }

    // Function to compute reflection direction


 float length_squared() const {
        return x * x + y * y + z * z;
    }
    

};

// Define the multiplication of a float and a Vec3 outside the class
inline Vec3 operator*(float scalar, const Vec3& vec) {
    return vec * scalar;
}

#endif // VEC3_H
