



#include <iostream>
#include <thread>  // For std::thread
#include <chrono>  // For std::chrono::seconds

int numCores = std::thread::hardware_concurrency();

// Function to be run on a separate thread
void printMessage() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Hello from the thread!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Pause for 1 second
    }
}

int main() {
    // Create a thread and pass the printMessage function to it
    
    std::cout << "Number of cores: " << numCores << std::endl;

    std::thread myThread(printMessage);

    // Print messages from the main function
    for (int i = 0; i < 5; ++i) {
        std::cout << "Hello from the main function!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Pause for 1 second
    }

    // Wait for the thread to finish
    myThread.join();

    return 0;
}
