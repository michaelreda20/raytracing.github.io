// ImageTexture.h

#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "Vec3.h"
#include <nlohmann/json.hpp>
#include "stb-master/stb-master/stb_image.h"  // Include the stb_image library
#include <string>
#include <iostream>
#include "color.h"


class ImageTexture {
public:
    // Texture image data
    unsigned char* data;
    int width, height, channels;
    


    ImageTexture() : data(nullptr), width(0), height(0), channels(0) {}
    ImageTexture(const std::string& filename) : data(nullptr), width(0), height(0), channels(0) {
        // Load image using stb_image library

        data = stbi_load(filename.c_str(), &width, &height, &channels, 0);


        // Ensure successful image loading before proceeding
        if (data == nullptr) {
            std::cerr << "Failed to load texture image: " << filename << std::endl;
        } else {
            std::cout << "Successfully loaded texture image: " << filename << std::endl;
        }
    }

ImageTexture(const nlohmann::json& materialJson) : data(nullptr), width(0), height(0), channels(0) {
    // Check if the "texture" key exists in the material JSON object
    if (materialJson.find("texture") != materialJson.end()) {
        // Check if the value associated with the "texture" key is a string
        if (materialJson["texture"].is_string() && materialJson.find("texture") != materialJson.end()) {
            // Extract the filename
            std::string filename = materialJson["texture"];

            
            std::cout << "Loading image: " << filename << std::endl;
            // Load image using stb_image library
            data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

        // Check for errors
        if (data == nullptr) {
            std::cerr << "Failed to load texture image: " << filename << "\n";
            std::cerr << "Error: " << stbi_failure_reason() << "\n";
        } else {
            std::cout << "Successfully loaded texture image: " << filename << "\n";
            std::cout << "Image dimensions: " << width << " x " << height << " channels: " << channels << "\n";
        }

        } else {
            std::cerr << "Value associated with 'texture' is either not a string or is an empty string" << std::endl;
        }
    } else {
        // Handle the case when the "texture" key is not present in the material JSON object
        std::cerr << "'texture' key not found in material JSON object" << std::endl;
    }
}
// getpixel function
Color getPixel(int x, int y) const {
    if (data == nullptr) {
       // std::cerr << "getPixel: data is nullptr!" << std::endl;
        // Return a default color or handle the error as needed
        return Color(0.0f, 0.0f, 0.0f);
    }

    // Adjust the calculation based on the number of channels
    int numChannels = channels;
    unsigned char* pixel = data + (y * width + x) * numChannels;

    float r = pixel[0] / 255.0f;
    float g = (numChannels > 1) ? pixel[1] / 255.0f : 0.0f;
    float b = (numChannels > 2) ? pixel[2] / 255.0f : 0.0f;

    return Color(r, g, b);
}



    // Sample texture color at given UV coordinates
  Vec3 sample(float u, float v) const {
    // Debugging output
   // std::cout << "Sampling texture at UV coordinates: (" << u << ", " << v << ")" << std::endl;
    u = std::clamp(u, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);
 //   std::cout << "sample1" << std::endl;


  int x0 = static_cast<int>(u * (width - 1));
int x1 = std::min(x0 + 1, width - 1);
int y0 = static_cast<int>(v * (height - 1));
int y1 = std::min(y0 + 1, height - 1);
 //   std::cout << "sample2" << std::endl;


float tx = u * (width - 1) - x0;
float ty = v * (height - 1) - y0;
 //   std::cout << "sample3" << std::endl;


// Perform bilinear interpolation
Color c00 = getPixel(x0, y0);
Color c01 = getPixel(x0, y1);
Color c10 = getPixel(x1, y0);
Color c11 = getPixel(x1, y1);
  //  std::cout << "sample4" << std::endl;


float r = (1 - tx) * (1 - ty) * c00.r + tx * (1 - ty) * c10.r + (1 - tx) * ty * c01.r + tx * ty * c11.r;
float g = (1 - tx) * (1 - ty) * c00.g + tx * (1 - ty) * c10.g + (1 - tx) * ty * c01.g + tx * ty * c11.g;
float b = (1 - tx) * (1 - ty) * c00.b + tx * (1 - ty) * c10.b + (1 - tx) * ty * c01.b + tx * ty * c11.b;
  //  std::cout << "sample5" << std::endl;


return Vec3(r, g, b);
}


    ~ImageTexture() {
        // Release memory when the texture is destroyed
        if (data != nullptr) {
            // Cleanup using stb_image library
            stbi_image_free(data);
        }
    }
};

#endif // IMAGE_TEXTURE_H
