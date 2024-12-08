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
    std::string baseName;

    std::vector<Vec3 *> projected_vertices;
    std::vector<Vec3 *> clipped_vertices;
    std::vector<Vec3 *> new_viewport_vertices;
    std::vector<std::vector<double>> zBuffer;


    // Constructor
    Scene(const char *xmlPath);

    // Main rendering pipeline
    void render();

    // Pipeline stages
    void transform_vertices_to_world(Mesh& mesh);
    void transform_vertices_to_camera(Camera& camera, Mesh& mesh);
    void project_camera_vertices(Camera& camera, Mesh& mesh);
    void perspective_divide(Mesh& mesh);
    void viewport_transform(Camera& camera, Mesh& mesh);
    void backface_culling(Camera& camera, Mesh& mesh);
    void rasterize(Camera& camera, Mesh& mesh);
    void drawLineWithZBuffer(int x0, int y0, double z0, int x1, int y1, double z1, const Color* color0, const Color* color1);
    
    void clip_wireframe_mesh(Mesh& mesh);
    std::vector<Vec4*> Scene::clip_triangle(Mesh& mesh, Triangle& triangle);
    // Utility functions
    void initializeImage(Camera *camera);
    int makeBetweenZeroAnd255(double value);
    void writeImageToPPMFile(Camera *camera);
    void convertPPMToPNG(std::string ppmFileName, int osType);
    // my utility functions
    void myWriteImageToPPMFile(Camera *camera);
    void myConvertPPMToPNG(std::string ppmFileName, int osType);
};

#endif // _SCENE_H_
