#ifndef IMAGE_WRITER_H
#define IMAGE_WRITER_H

#include "Vec3.h"
#include "Color.h"  // Include the Color class for the clamp function
#include <iostream>
#include <fstream>

class ImageWriter {
public:
    static void writePPM(const char* filename, int width, int height, const Vec3* image) {
        std::ofstream file(filename);
        file << "P3\n" << width << " " << height << "\n255\n";

        for (int j = height - 1; j >= 0; --j) {
            for (int i = width-1; i >= 0; --i) {
                int index = j * width + i;

                // Use the clamp function from the Color class to ensure color values are in [0, 1]
                Color clampedColor(image[index].x, image[index].y, image[index].z);
                clampedColor.clamp();

                int r = static_cast<int>(255.99f * clampedColor.r);
                int g = static_cast<int>(255.99f * clampedColor.g);
                int b = static_cast<int>(255.99f * clampedColor.b);

                file << r << " " << g << " " << b << "\n";
            }
        }

        std::cout << "Image generated: " << filename << std::endl;
    }
};

#endif // IMAGE_WRITER_H
