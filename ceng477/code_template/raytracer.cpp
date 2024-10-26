#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>
#include <vector>
#include "parser.h"

using namespace parser;
using namespace std;

typedef unsigned char RGB[3];

void test(){

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.

    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };

    int width = 640, height = 480;
    int columnWidth = width / 8;

    unsigned char* image = new unsigned char [width * height * 3];

    int i = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    }

    write_ppm("test.ppm", image, width, height);
}


// Function to normalize a vector
Vec3f normalize(const Vec3f& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return { v.x / length, v.y / length, v.z / length };
}

// Function to compute cross product
Vec3f cross(const Vec3f& a, const Vec3f& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Function to negate a vector (reverse direction)
Vec3f negate(const Vec3f& v) {
    return { -v.x, -v.y, -v.z };
}

void printVec3f(const std::string& prefix, const Vec3f& v) {
    std::cout << prefix << ": " <<  v.x << " " << v.y << " " << v.z << std::endl;
}

// Function to generate a ray for a pixel (i, j)
Vec3f generateRay(const Camera& camera, int i, int j) {
    // Reverse gaze to get -w direction (into the scene)
    // printVec3f("gaze", camera.gaze);
    Vec3f gaze = normalize(camera.gaze);  // Now gaze points into the scene (along -w)
    // printVec3f("normalized gaze", gaze);

    // printVec3f("up", camera.up);
    Vec3f up = normalize(camera.up);
    // printVec3f("normalized up", up);
    // Corrected cross product: right vector (u = up × -gaze)
    Vec3f right = cross(up, gaze);  // Correct formula as per homework convention
    right = normalize(right);

    // Camera parameters
    float left = camera.near_plane.x;
    float right_plane = camera.near_plane.y;
    float bottom = camera.near_plane.z;
    float top = camera.near_plane.w;
    float near_distance = camera.near_distance;

    int image_width = camera.image_width;
    int image_height = camera.image_height;

    // Calculate u and v for pixel (i, j)
    float u = left + (right_plane - left) * (i + 0.5f) / image_width;
    float v = bottom + (top - bottom) * (j + 0.5f) / image_height;

    // Calculate ray direction from camera through pixel (i, j)
    Vec3f ray_direction = {
        near_distance * gaze.x + u * right.x + v * up.x,
        near_distance * gaze.y + u * right.y + v * up.y,
        near_distance * gaze.z + u * right.z + v * up.z
    };

    return normalize(ray_direction);
    return {0, 0, 0};
}

vector<vector<Vec3f>> processImageRays(const parser::Camera& camera) {
    
    vector<vector<Vec3f>> image_rays(camera.image_height, vector<Vec3f>(camera.image_width));
    // Loop over each pixel in the image
    for (int j = 0; j < camera.image_height; ++j) {      // Vertical axis (rows)
        for (int i = 0; i < camera.image_width; ++i) {   // Horizontal axis (columns)
            // Generate a ray for pixel (i, j)
            parser::Vec3f ray_direction = generateRay(camera, i, j);
            // For debugging, you can print out each generated ray direction
            printVec3f("Ray direction", ray_direction);
            image_rays[j][i] = ray_direction;
            // You can store or use the ray for intersection tests with objects in the scene
        }
    }

    return image_rays;
}




int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    // parser::Scene scene;

    // scene.loadFromXml(argv[1]);

    // ray tracing here
    // test();
    
    parser::Camera camera;
    camera.position = {0.0f, 0.0f, 0.0f};       // Camera at origin
    camera.gaze = {0.0f, 0.0f, -1.0f};          // Looking towards negative z
    camera.up = {0.0f, 1.0f, 0.0f};             // Up along y-axis
    camera.near_plane = {-1.0f, 1.0f, -1.0f, 1.0f};  // Image plane boundaries (left, right, bottom, top)
    camera.near_distance = 1.0f;                // Distance from camera to image plane
    camera.image_width = 1024;
    camera.image_height = 768;
    camera.image_name = "output.ppm";

    // processImageRays(camera);
    // slide example for testing
    // const Vec3f ray = generateRay(camera, 256, 192);
    // printVec3f("Ray", ray);

    vector<vector<Vec3f>> imageRays = processImageRays(camera);


    return 0;


}
