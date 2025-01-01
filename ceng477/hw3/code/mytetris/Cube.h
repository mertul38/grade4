#ifndef CUBE_H
#define CUBE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include "Common.h"

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
