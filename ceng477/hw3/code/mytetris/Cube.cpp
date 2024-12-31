#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#define _USE_MATH_DEFINES


#include <math.h>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;


class Cube {
public:
    static GLuint vao;
    static int gTriangleIndexDataSizeInBytes;

    glm::mat4 modelMatrix;

    Cube(glm::vec3 position = glm::vec3(-0.5, -0.5, -0.5)) {
        modelMatrix = glm::translate(glm::mat4(1.f), position);
    }

    static void init(GLuint& vertexBuffer, GLuint& indexBuffer) {
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
            0, 1, 2, // front
            3, 0, 2, // front
            4, 7, 6, // back
            5, 4, 6, // back
            0, 3, 4, // left
            3, 7, 4, // left
            2, 1, 5, // right
            6, 2, 5, // right
            3, 2, 7, // top
            2, 6, 7, // top
            0, 4, 1, // bottom
            4, 5, 1  // bottom
        };

        GLuint indicesLines[] = {
            7, 3, 2, 6, // top
            4, 5, 1, 0, // bottom
            2, 1, 5, 6, // right
            5, 4, 7, 6, // back
            0, 1, 2, 3, // front
            0, 3, 7, 4, // left
        };

        GLfloat vertexPos[] = {
            0, 0, 1, // 0: bottom-left-front
            1, 0, 1, // 1: bottom-right-front
            1, 1, 1, // 2: top-right-front
            0, 1, 1, // 3: top-left-front
            0, 0, 0, // 4: bottom-left-back
            1, 0, 0, // 5: bottom-right-back
            1, 1, 0, // 6: top-right-back
            0, 1, 0, // 7: top-left-back
        };

        GLfloat vertexNor[] = {
            1.0,  1.0,  1.0, // 0: unused
            0.0, -1.0,  0.0, // 1: bottom
            0.0,  0.0,  1.0, // 2: front
            1.0,  1.0,  1.0, // 3: unused
            -1.0,  0.0,  0.0, // 4: left
            1.0,  0.0,  0.0, // 5: right
            0.0,  0.0, -1.0, // 6: back 
            0.0,  1.0,  0.0, // 7: top
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


    void drawFaces(GLuint modelingMatrixLoc) {
        glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    void drawEdges(GLuint modelingMatrixLoc) {
        glLineWidth(3);

        glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        for (int i = 0; i < 6; ++i) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(gTriangleIndexDataSizeInBytes + i * 4 * sizeof(GLuint)));
        }
    }
};

GLuint Cube::vao = 0;
int Cube::gTriangleIndexDataSizeInBytes = 0;
