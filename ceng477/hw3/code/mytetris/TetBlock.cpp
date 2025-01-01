// TetBlock.cpp

#include "Cube.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TetBlock {
public:
    std::vector<Cube> cubes;

    TetBlock(glm::vec3 position) {
        init(position);
    }

    void init(glm::vec3 position) {
        cubes.clear();
        // Define all relative positions for the 27 cubes in a 3x3x3 grid
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                for (int z = -1; z <= 1; ++z) {
                    cubes.emplace_back(position + glm::vec3(x, y, z));
                }
            }
        }
    }

    void drawFaces() {
        for (auto& cube : cubes) {
            cube.drawFaces();
        }
    }

    void drawEdges() {
        for (auto& cube : cubes) {
            cube.drawEdges();
        }
    }

    Cube& getCenterCube() {
        // The center cube in a 3x3x3 grid initialized in sequential order is at index 13
        return cubes[13];
    }

    glm::vec3 getPosition() {
        Cube& centerCube = getCenterCube();
        // Assuming modelToPosition is a function in Cube that converts model matrix to position
        glm::vec3 position = centerCube.modelToPosition(centerCube.modelMatrix);
        return position;
    }

    void move(glm::vec3 moveVector) {
        for (auto& cube : cubes) {
            auto proposalModelMatrix = cube.getProposalModelMatrix(moveVector);
            cube.move(proposalModelMatrix);
        }
    }
};

// This single file combines the TetBlock class implementation and should be included
// in any project file that requires using TetBlock, with the correct linking of Cube and GLM dependencies.
