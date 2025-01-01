#ifndef TETBLOCK_H
#define TETBLOCK_H

#include "Cube.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TetBlock {
public:
    bool landed;
    bool on_ground;
    std::vector<Cube> cubes;

    // Constructor
    TetBlock(glm::vec3 position);

    // Member functions
    void init(glm::vec3 position);
    void drawFaces();
    void drawEdges();
    Cube& getCenterCube();
    glm::vec3 getPosition();
    void move(glm::vec3 moveVector);
};

#endif // TETBLOCK_H
