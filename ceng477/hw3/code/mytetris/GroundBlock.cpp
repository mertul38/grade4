// GroundBlock.cpp

#include <vector>
#include <glm/glm.hpp>
#include "Cube.h"

class GroundBlock {
public:
    std::vector<Cube> cubes;

    GroundBlock(glm::vec3 position) {
        init(position);
    }

    void init(glm::vec3 position) {
        cubes.clear();
        // Define the relative positions for the 9x9 grid of cubes
        int halfSize = 4;  // Half the size of the grid (9 / 2 = 4.5, approximated to 4)
        for (int x = -halfSize; x <= halfSize; ++x) {
            for (int z = -halfSize; z <= halfSize; ++z) {
                glm::vec3 offset(x, 0, z);  // Spread on the X and Z axes
                cubes.emplace_back(position + offset);
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


};

// You can include this in any .cpp file that requires using GroundBlock
// Just make sure to link the necessary dependencies for compilation.
