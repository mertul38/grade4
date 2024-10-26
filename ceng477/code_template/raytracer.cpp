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

void test_slideexample() {

    Camera camera = {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 1.0f, 0.0f},
        {-1.0f, 1.0f, -1.0f, 1.0f},
        1.0f,
        1024,
        768,
        "slide.ppm"
    };
    camera.calculateAdditiveCameraParameters();

    const Ray& ray = camera.generateRay(256, 192);
    cout << "Ray[192][256]: " << ray.toString() << endl;

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


void actualRayTracing(int argc, char* argv[]) {
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    cout << "Loading scene from " << argv[1] << endl;
    scene.loadFromXml(argv[1]);

    cout << "Calculating Additive Parameters" << endl;
    scene.calculateAdditiveParameters();
}

int main(int argc, char* argv[])
{

    actualRayTracing(argc, argv);
    // test_slideexample();
    return 0;


}
