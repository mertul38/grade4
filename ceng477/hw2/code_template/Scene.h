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
    void projectVertices(Camera *camera);
    void backfaceCulling(Camera *camera, Mesh *mesh);
    void clipTriangles(Camera *camera, Mesh *mesh);
    void mapToViewport(Camera *camera, Mesh *mesh);
    void rasterizeTriangles(Camera *camera, Mesh *mesh);

    // Helper functions
    bool isBackface(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3, const Vec3 &cameraPosition);
    void transformVerticesToCamera(Camera *camera);
    void drawLine(Vec3 *v1, Vec3 *v2, std::vector<std::vector<double>> &depthBuffer, Camera *camera);
    void updateDepthBuffer(int x, int y, float depth, Color color, std::vector<std::vector<double>> &depthBuffer, Camera *camera);
    void fillTriangle(Vec3 *v1, Vec3 *v2, Vec3 *v3, std::vector<std::vector<double>> &depthBuffer, Camera *camera);
    double edgeFunction(Vec3 *v1, Vec3 *v2, Vec3 *p);
    Color interpolateColor(double w1, double w2, double w3, int colorId1, int colorId2, int colorId3);

    // Utility functions
    void initializeImage(Camera *camera);
    int makeBetweenZeroAnd255(double value);
    void writeImageToPPMFile(Camera *camera);
    void convertPPMToPNG(std::string ppmFileName, int osType);
};

#endif // _SCENE_H_
