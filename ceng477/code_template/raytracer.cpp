#include <iostream>
#include "parser.h"
#include <cmath>
#include <vector>


using namespace parser;
using namespace std;

typedef unsigned char RGB[3];

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
    // camera.calculateAdditiveInfo();

    const Ray& ray = camera.generateRay(256, 192);
    cout << "Ray[192][256]: " << ray.toString() << endl;

}


void actualRayTracing(int argc, char* argv[]) {
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    cout << "Loading scene from " << argv[1] << endl;
    scene.loadFromXml(argv[1]);

    cout << "Calculating Additive Parameters" << endl;
    scene.calculateAdditiveInfo();
    scene.render();
}

int main(int argc, char* argv[])
{

    actualRayTracing(argc, argv);
    // test_slideexample();
    return 0;


}
