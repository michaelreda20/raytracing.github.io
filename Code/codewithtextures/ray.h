#ifndef RAY_H
#define RAY_H

#include "Vec3.h"
#include <iostream>
using namespace std;


class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {}

    Vec3 at(float t) const {
    return origin + t * direction;

    
}

// function to print ray


};

#endif // RAY_H


