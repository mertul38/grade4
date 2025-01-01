#include "Cuboid.h"


int Cuboid::cuboidVertexDataSize = 0;
int Cuboid::cuboidNormalDataSize = 0;
int Cuboid::cuboidIndexDataSize = 0;
int Cuboid::cuboidLineIndexDataSize = 0;
void Cuboid::init(GLuint& vao, GLuint& vertexBuffer, GLuint& indexBuffer, float heightScale) {

    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0); // Enable attribute 0 (positions)
    glEnableVertexAttribArray(1); // Enable attribute 1 (normals)

    // Generate and bind buffers for vertex and index data
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);


    GLfloat vertexPos[] = {
        0, 0, 1, 1, 0, 1, 1, heightScale, 1, 0, heightScale, 1, 
        0, 0, 0, 1, 0, 0, 1, heightScale, 0, 0, heightScale, 0
    };

    GLfloat vertexNor[] = {
        1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0,
        -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0
    };

    GLuint indices[] = {
        0, 1, 2, 3, 0, 2, 4, 7, 6, 5, 4, 6,
        0, 3, 4, 3, 7, 4, 2, 1, 5, 6, 2, 5,
        3, 2, 7, 2, 6, 7, 0, 4, 1, 4, 5, 1
    };

    GLuint indicesLines[] = {
        7, 3, 2, 6, 4, 5, 1, 0, 2, 1, 5, 6,
        5, 4, 7, 6, 0, 1, 2, 3, 0, 3, 7, 4
    };

    cuboidVertexDataSize = sizeof(vertexPos);
    cuboidNormalDataSize = sizeof(vertexNor);
    cuboidIndexDataSize = sizeof(indices);
    cuboidLineIndexDataSize = sizeof(indicesLines);

    // Bind the vertex buffer and upload vertex and normal data
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, cuboidVertexDataSize + cuboidNormalDataSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cuboidVertexDataSize, vertexPos);
    glBufferSubData(GL_ARRAY_BUFFER, cuboidVertexDataSize, cuboidNormalDataSize, vertexNor);

    // Bind the index buffer and upload triangle and line indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cuboidIndexDataSize + cuboidLineIndexDataSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, cubeIndexDataSize, indices);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cubeIndexDataSize, cubeLineIndexDataSize, indicesLines);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Attribute 0: position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cuboidVertexDataSize)); // Attribute 1: normal
}

void Cuboid::drawFaces() {
    glUniformMatrix4fv(modelingMatrixLoc_face, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cuboid::drawEdges() {
    glLineWidth(3);
    glUniformMatrix4fv(modelingMatrixLoc_edge, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    for (int i = 0; i < 6; ++i) {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(cuboidIndexDataSize + i * 4 * sizeof(GLuint)));
    }
}

Cuboid::Cuboid(glm::vec3 position, float heightScale)
    : Cube(position), heightScale(heightScale) {
    modelMatrix = glm::translate(glm::mat4(1.0f), position);
}

