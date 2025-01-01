#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <math.h>



using namespace std;

#include "Cube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include <glm/gtc/constants.hpp> // For glm::epsilon()

#define EPSILON 0.0001

GLuint Cube::vao = 0;
GLint Cube::modelingMatrixLoc_face = 0;
GLint Cube::modelingMatrixLoc_edge = 0;
int Cube::gTriangleIndexDataSizeInBytes = 0;

void Cube::init(GLuint& vertexBuffer, GLuint& indexBuffer) {
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    assert(vertexBuffer > 0 && indexBuffer > 0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    GLuint indices[] = {
        0, 1, 2, 3, 0, 2, 4, 7, 6, 5, 4, 6, 0, 3, 4, 3, 7, 4, 2, 1, 5, 6, 2, 5, 3, 2, 7, 2, 6, 7, 0, 4, 1, 4, 5, 1
    };

    GLuint indicesLines[] = {
        7, 3, 2, 6, 4, 5, 1, 0, 2, 1, 5, 6, 5, 4, 7, 6, 0, 1, 2, 3, 0, 3, 7, 4
    };

    GLfloat vertexPos[] = {
        0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0
    };

    GLfloat vertexNor[] = {
        1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0
    };

    int gVertexDataSizeInBytes = sizeof(vertexPos);
    int gNormalDataSizeInBytes = sizeof(vertexNor);
    gTriangleIndexDataSizeInBytes = sizeof(indices);
    int gLineIndexDataSizeInBytes = sizeof(indicesLines);
    int allIndexSize = gTriangleIndexDataSizeInBytes + gLineIndexDataSizeInBytes;

    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexPos);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, vertexNor);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndexSize, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, gTriangleIndexDataSizeInBytes, indices);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gTriangleIndexDataSizeInBytes, gLineIndexDataSizeInBytes, indicesLines);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}

glm::vec3 Cube::modelToPosition(const glm::mat4& modelMatrix) {
    return glm::vec3(modelMatrix[3]) + glm::vec3(0.5f, 0.5f, 0.5f);
}

Cube::Cube(glm::vec3 position) {
    // add -0.5 to center the cube
    position -= glm::vec3(0.5f, 0.5f, 0.5f);
    modelMatrix = glm::translate(glm::mat4(1.0f), position);
}

void Cube::drawFaces() {
    glUniformMatrix4fv(modelingMatrixLoc_face, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::drawEdges() {
    glLineWidth(3);
    glUniformMatrix4fv(modelingMatrixLoc_edge, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    for (int i = 0; i < 6; ++i) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(gTriangleIndexDataSizeInBytes + i * 4 * sizeof(GLuint)));
    }
}

glm::vec3 Cube::getMoveVector(MoveDirection direction, const glm::vec3& eyePos) {
    glm::vec3 moveVector;

    if (direction == MoveDirection::Down) {
        // Move down on the y-axis
        moveVector = glm::vec3(0, -1, 0);
    }
    else{
        // Normalize eyePos to avoid any scaling issues
        glm::vec3 normalizedEyePos = glm::normalize(eyePos);
        // Determine movement direction based on eyePos and desired move direction
        if (glm::abs(normalizedEyePos.z) > EPSILON) { // Looking along Z
            if (normalizedEyePos.z > 0) { // Facing +Z
                moveVector = (direction == MoveDirection::Left) ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0); // Move +X if left, -X if right
            } else { // Facing -Z
                moveVector = (direction == MoveDirection::Left) ? glm::vec3(1, 0, 0) : glm::vec3(-1, 0, 0); // Move -X if left, +X if right
            }
        } else if (glm::abs(normalizedEyePos.x) > EPSILON) { // Looking along X
            if (normalizedEyePos.x > 0) { // Facing +X
                moveVector = (direction == MoveDirection::Left) ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1); // Move -Z if left, +Z if right
            } else { // Facing -X
                moveVector = (direction == MoveDirection::Left) ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1); // Move +Z if left, -Z if right
            }
        }
    }

    return moveVector;
}

glm::mat4 Cube::getProposalModelMatrix(glm::vec3 moveVector) {
    glm::mat4 proposalModelMatrix = glm::translate(modelMatrix, moveVector);
    return proposalModelMatrix;
}

void Cube::move(const glm::mat4& modelMatrix) {
    this->modelMatrix = modelMatrix;
}
