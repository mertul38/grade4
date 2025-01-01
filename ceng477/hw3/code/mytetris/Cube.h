#ifndef CUBE_H
#define CUBE_H

#include <GL/glew.h>
#include <glm/glm.hpp>


class Cube {
public:
    enum MoveDirection{ Left, Right, Down};

    static GLuint vao;
    static GLint modelingMatrixLoc_face;
    static GLint modelingMatrixLoc_edge;
    static int gTriangleIndexDataSizeInBytes;

    static void init(GLuint& vertexBuffer, GLuint& indexBuffer);

    static glm::vec3 modelToPosition(const glm::mat4& modelMatrix);
    
    Cube(glm::vec3 position = glm::vec3(-0.5, -0.5, -0.5));
    void drawFaces();
    void drawEdges();
    void move(const glm::mat4& modelMatrix);
    glm::vec3 getMoveVector(MoveDirection direction, const glm::vec3& eyePos);
    glm::mat4 getProposalModelMatrix(glm::vec3 moveVector);
    glm::mat4 modelMatrix;


};

#endif // CUBE_H
