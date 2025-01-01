#ifndef CUBOID_H
#define CUBOID_H

#include "Cube.h"
#include "Common.h"

class Cuboid : public Cube {
public:
    static GLuint vao;  // Separate VAO for Cuboid
    static void init(GLuint& vertexBuffer, GLuint& indexBuffer, float heightScale);

    Cuboid(glm::vec3 position = glm::vec3(-0.5f, -0.5f, -0.5f), float heightScale = 0.5f);

private:
    float heightScale;
};

#endif // CUBOID_H
