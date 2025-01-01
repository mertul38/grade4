#include "Cuboid.h"


GLuint Cuboid::vao = 0;
void Cuboid::init(GLuint& vertexBuffer, GLuint& indexBuffer, float heightScale) {
    glGenVertexArrays(1, &vao); // Generate and bind a Vertex Array Object (VAO)
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

    int vertexDataSize = sizeof(vertexPos);
    int normalDataSize = sizeof(vertexNor);
    int indexDataSize = sizeof(indices);
    int lineIndexDataSize = sizeof(indicesLines);

    // Bind the vertex buffer and upload vertex and normal data
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSize, vertexPos);
    glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, normalDataSize, vertexNor);

    // Bind the index buffer and upload triangle and line indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize + lineIndexDataSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexDataSize, indices);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, lineIndexDataSize, indicesLines);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Attribute 0: position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(intptr_t)vertexDataSize); // Attribute 1: normal
}


Cuboid::Cuboid(glm::vec3 position, float heightScale)
    : Cube(position), heightScale(heightScale) {
    modelMatrix = glm::translate(glm::mat4(1.0f), position);
}

