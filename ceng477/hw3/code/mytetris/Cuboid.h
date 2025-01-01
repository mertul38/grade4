#ifndef CUBOID_H
#define CUBOID_H

#include "Cube.h"
#include "Common.h"

class Cuboid : public Cube {

public:

    static int cuboidVertexDataSize;
    static int cuboidNormalDataSize;
    static int cuboidIndexDataSize;
    static int cuboidLineIndexDataSize;

    static void init(GLuint& vao, GLuint& vertexBuffer, GLuint& indexBuffer, float heightScale);

    Cuboid(glm::vec3 position = glm::vec3(-0.5f, -0.5f, -0.5f), float heightScale = 0.5f);
    void drawFaces();
    void drawEdges();

private:
    float heightScale;
};

#endif // CUBOID_H
