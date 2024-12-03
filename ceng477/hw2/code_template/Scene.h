#ifndef _SCENE_H_
#define _SCENE_H_

#include "Vec3.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"
#include "Helpers.h"
#include <vector>

class Scene
{
public:
    // Attributes
    Color backgroundColor;
    bool cullingEnabled;

    std::vector<std::vector<Color>> image;
    std::vector<Camera *> cameras;
    std::vector<Vec3 *> vertices;
    std::vector<Color *> colorsOfVertices;
    std::vector<Scaling *> scalings;
    std::vector<Rotation *> rotations;
    std::vector<Translation *> translations;
    std::vector<Mesh *> meshes;

    // MY ADDINGS
    std::vector<Vec3 *> world_vertices;
    std::vector<Vec3 *> projected_vertices;
    std::vector<Vec3 *> clipped_vertices;
    std::vector<Vec3 *> new_viewport_vertices;

    // Constructor
    Scene(const char *xmlPath);

    // Main rendering pipeline
    void forwardRenderingPipeline(Camera *camera);

    // Pipeline stages
    void transformVerticesToWorld();

    // Utility functions
    void initializeImage(Camera *camera);
    int makeBetweenZeroAnd255(double value);
    void writeImageToPPMFile(Camera *camera);
    void convertPPMToPNG(std::string ppmFileName, int osType);
};

#endif // _SCENE_H_
