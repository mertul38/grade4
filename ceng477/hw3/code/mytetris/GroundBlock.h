#ifndef GROUNDBLOCK_H
#define GROUNDBLOCK_H

#include <vector>
#include <glm/glm.hpp>
#include "Cube.h"

class GroundBlock {
public:
    std::vector<Cube> cubes;

    // Constructor
    GroundBlock(glm::vec3 position);

    // Member functions
    void init(glm::vec3 position);
    void drawFaces();
    void drawEdges();
};

#endif // GROUNDBLOCK_H
