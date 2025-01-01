#include "TetBlock.h"

// Constructor implementation
TetBlock::TetBlock(glm::vec3 position)
    : landed(false), on_ground(false) {
    init(position);
}

// Initialize the TetBlock with a 3x3x3 grid of cubes
void TetBlock::init(glm::vec3 position) {
    cubes.clear();
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                cubes.emplace_back(position + glm::vec3(x, y, z));
            }
        }
    }
}

// Draw all faces of the cubes
void TetBlock::drawFaces() {
    for (auto& cube : cubes) {
        cube.drawFaces();
    }
}

// Draw all edges of the cubes
void TetBlock::drawEdges() {
    for (auto& cube : cubes) {
        cube.drawEdges();
    }
}

// Get the center cube of the 3x3x3 grid
Cube& TetBlock::getCenterCube() {
    return cubes[13]; // The center cube in a 3x3x3 grid initialized in sequential order
}

// Get the position of the center cube
glm::vec3 TetBlock::getPosition() {
    Cube& centerCube = getCenterCube();
    return centerCube.modelToPosition(centerCube.modelMatrix); // Assuming this method exists in Cube
}

// Move all cubes in the TetBlock
void TetBlock::move(glm::vec3 moveVector) {
    for (auto& cube : cubes) {
        auto proposalModelMatrix = cube.getProposalModelMatrix(moveVector);
        cube.move(proposalModelMatrix); // Assuming move function exists in Cube
    }
}
