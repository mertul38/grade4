#include <iostream>
#include <thread>

int main() {
    // Get the number of CPU cores or logical cores supported by the hardware
    unsigned int numCores = std::thread::hardware_concurrency();

    // Print the number of cores
    if (numCores > 0) {
        std::cout << "Number of CPU cores: " << numCores << std::endl;
    } else {
        std::cout << "Could not determine the number of CPU cores." << std::endl;
    }

    return 0;
}
