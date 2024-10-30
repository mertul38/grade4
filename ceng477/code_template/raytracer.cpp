#include <iostream>
#include "parser.h"
#include <cmath>
#include <vector>


using namespace parser;
using namespace std;

typedef unsigned char RGB[3];

void test_rayGeneration() {

    const Vec3f expected_direction = {-0.4077604, 0.40749441, -0.81711673};

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
    camera.calculateAdditiveInfo();
    const Ray& ray = camera.generateRay(256, 192);
    cout << "Ray[192][256]: " << ray.toString() << endl;
    if (compareFloats(ray.direction.x, expected_direction.x) && compareFloats(ray.direction.y, expected_direction.y) && compareFloats(ray.direction.z, expected_direction.z)) {
        cout << "Test passed" << endl;
    }
    else {
        cout << "Test failed" << endl;
    }

}


void actualRayTracing(int argc, char* argv[]) {
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    cout << "Creating scene for " << argv[1] << endl;
    scene.loadFromXml(argv[1]);

    scene.calculateAdditiveInfo();
    scene.render_multithreaded();
    cout << "---------------------------" << endl;
}

int main(int argc, char* argv[])
{

    // test_rayGeneration();
    actualRayTracing(argc, argv);
    return 0;


}
