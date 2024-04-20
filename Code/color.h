// Color.h
#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

class Color {
public:
    float r, g, b;

    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    Color(float red, float green, float blue) : r(red), g(green), b(blue) {}

    // Clamp color values to the range [0, 1]
    void clamp() {
        r = (r < 0.0f) ? 0.0f : (r > 1.0f) ? 1.0f : r;
        g = (g < 0.0f) ? 0.0f : (g > 1.0f) ? 1.0f : g;
        b = (b < 0.0f) ? 0.0f : (b > 1.0f) ? 1.0f : b;
    }

    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b);
    }

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }

    Color& operator+=(const Color& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    Color operator/(float scalar) const {
        // Ensure scalar is not zero to avoid division by zero
        if (scalar != 0.0f) {
            float invScalar = 1.0f / scalar;
            return Color(r * invScalar, g * invScalar, b * invScalar);
        }
        // Handle division by zero gracefully, perhaps by returning a default color
        return Color();
    }

    Color& operator/=(float scalar) {
        // Ensure scalar is not zero to avoid division by zero
        if (scalar != 0.0f) {
            float invScalar = 1.0f / scalar;
            r *= invScalar;
            g *= invScalar;
            b *= invScalar;
        }
        // Handle division by zero gracefully, perhaps by returning a default color
        return *this;
    }

    

    Color& operator*=(float scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }

    


};

#endif // COLOR_H
