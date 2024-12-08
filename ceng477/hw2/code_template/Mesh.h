#ifndef __MESH_H__
#define __MESH_H__
#define WIREFRAME_MESH 0
#define SOLID_MESH 1
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"
#include <vector>

class Mesh
{

public:
    int meshId, type, numberOfTransformations, numberOfTriangles; // type=0 for wireframe, type=1 for solid
    std::vector<int> transformationIds;
    std::vector<char> transformationTypes;
    std::vector<Triangle> triangles;

    // MY ADDINGS
    std::vector<Triangle* > world_triangles;
    std::vector<Triangle* > clipped_triangles;

    std::vector<Vec3*> world_vertices;
    std::vector<Vec3*> camera_vertices;
    std::vector<Vec4*> projected_vertices;
    std::vector<Vec4*> clipped_vertices;
    std::vector<Vec3*> perspected_vertices;
    std::vector<Vec3*> viewport_vertices;

    std::vector<bool> culled_triangles;

    
    void clear();

    Mesh();
    Mesh(int meshId, int type, int numberOfTransformations,
         std::vector<int> transformationIds,
         std::vector<char> transformationTypes,
         int numberOfTriangles,
         std::vector<Triangle> triangles);

    friend std::ostream &operator<<(std::ostream &os, const Mesh &m);
};

#endif