#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>
#include <vector>
#include "parser.h"

using namespace parser;
using namespace std;

typedef unsigned char RGB[3];

float EPSILON = 1e-6;


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

// Function to generate a ray for a pixel (i, j)
void calculateTriangleValues(Scene& scene){
    for (int i = 0; i < scene.triangles.size(); i++) {
        Triangle& triangle = scene.triangles[i];
        Vec3f& v0 = scene.vertex_data[triangle.indices.v0_id];
        Vec3f& v1 = scene.vertex_data[triangle.indices.v1_id];
        Vec3f& v2 = scene.vertex_data[triangle.indices.v2_id];

        triangle.E1 = v1 - v0;
        triangle.E2 = v2 - v0;
    }
}

Ray generateRay(const Camera& camera, int i, int j) {
    // Reverse gaze to get -w direction (into the scene)
    // printVec3f("gaze", camera.gaze);
    Vec3f gaze = camera.gaze.normalize();  // Now gaze points into the scene (along -w)
    // printVec3f("normalized gaze", gaze);

    // printVec3f("up", camera.up);
    Vec3f up = camera.up.normalize();
    // printVec3f("normalized up", up);
    // Corrected cross product: right vector (u = up × -gaze)
    Vec3f right = up.cross(gaze.negate());  // Correct formula as per homework convention
    right = right.normalize();

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

    Ray ray = {camera.position, ray_direction};

    return ray;
}

vector<vector<Ray>> processImageRays(const Camera& camera) {
    vector<vector<Ray>> image_rays(camera.image_height, vector<Ray>(camera.image_width));
    // Loop over each pixel in the image
    for (int j = 0; j < camera.image_height; ++j) {      // Vertical axis (rows)
        for (int i = 0; i < camera.image_width; ++i) {   // Horizontal axis (columns)
            // Generate a ray for pixel (i, j)
            Ray ray = generateRay(camera, i, j);
            // For debugging, you can print out each generated ray direction
            image_rays[j][i] = ray;
            // You can store or use the ray for intersection tests with objects in the scene
        }
    }

    return image_rays;
}

int rayTriangleIntersection(
    Scene& scene,
    Vec3f& ray,
    Vec3f& ray_origin,
    Triangle& triangle
    ) {

    // Get the vertices of the triangle
    Vec3f& v0 = scene.vertex_data[triangle.indices.v0_id];
    Vec3f& v1 = scene.vertex_data[triangle.indices.v1_id];
    Vec3f& v2 = scene.vertex_data[triangle.indices.v2_id];

    Vec3f E1 = v1 - v0;
    Vec3f E2 = v2 - v0;
    Vec3f T = ray_origin - v0;
    Vec3f P = ray.cross(E2);
    Vec3f Q = T.cross(E1);

    float det = P.dot(E1);

    // backface culling
    if (det < 0) {
        return 0;
    }
    // check if ray and triangle are parallel
    if (det < EPSILON) {
        return 0;
    }

    // calculate u
    float u = P.dot(T) / det;
    if (u < 0 || u > 1) {
        return 0;
    }

    // calculate v
    float v = Q.dot(ray) / det;
    if (v < 0 || u + v > 1) {
        return 0;
    }

    // calculate t
    float t = Q.dot(E2) / det;
    if (t < 0) {
        return 0;
    }

    return t;
}




int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    scene.loadFromXml(argv[1]);

    calculateTriangleValues(scene);

    // Vec3f ray_origin = {0, 0, 0};
    // Vec3f ray_direction = {0, 0, 1};
    // Ray ray = {ray_origin, ray_direction};

    // for each camera
    // for(int camera_i = 0; camera_i < scene.cameras.size(); camera_i++) {
    //     Camera& camera = scene.cameras[camera_i];
    //     vector<vector<Vec3f>> imageRays = processImageRays(camera);
    //     // for each pixel
    //     for (int j = 0; j < camera.image_height; ++j) {
    //         for (int i = 0; i < camera.image_width; ++i) {
    //             Vec3f ray = imageRays[j][i];
    //             // for each triangle
                

    //             break;
    //         }
    //     break;
    //     }
    // }

    return 0;


}
