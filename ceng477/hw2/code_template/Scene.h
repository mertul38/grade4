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

    // Constructor
    Scene(const char *xmlPath);

    // Main rendering pipeline
    void forwardRenderingPipeline(Camera *camera);

    // Pipeline stages
    void transformVerticesToWorld(Mesh *mesh);
    void transformVerticesToCamera(Mesh *mesh, Camera *camera);
    void projectVertices(Mesh *mesh, Camera *camera);
    void clipTriangles(Mesh *mesh);
    void mapToViewport(Mesh *mesh, Camera *camera);
    void rasterizeTriangles(Mesh *mesh, Camera *camera);

    // Utility for depth buffering (optional for rasterization)
    void updateDepthBuffer(int x, int y, float depth, Color color);

    // Utility functions
    void initializeImage(Camera *camera);
    int makeBetweenZeroAnd255(double value);
    void writeImageToPPMFile(Camera *camera);
    void convertPPMToPNG(std::string ppmFileName, int osType);
};

#endif // _SCENE_H_
