#include "GroundBlock.h"

// Constructor implementation
GroundBlock::GroundBlock(glm::vec3 position) {
    init(position);
}

// Initialize the ground block with a 9x9 grid of cubes
void GroundBlock::init(glm::vec3 position) {
    cubes.clear();
    int halfSize = 4;  // Half the size of the grid (9 / 2 = 4.5, approximated to 4)
    for (int x = -halfSize; x <= halfSize; ++x) {
        for (int z = -halfSize; z <= halfSize; ++z) {
            glm::vec3 offset(x, 0, z);  // Spread on the X and Z axes
            cubes.emplace_back(position + offset);
        }
    }
}

// Draw all faces of the cubes
void GroundBlock::drawFaces() {
    for (auto& cube : cubes) {
        cube.drawFaces();
    }
}

// Draw all edges of the cubes
void GroundBlock::drawEdges() {
    for (auto& cube : cubes) {
        cube.drawEdges();
    }
}
