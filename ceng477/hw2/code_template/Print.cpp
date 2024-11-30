#include "Print.h"
#include <iostream>
#include <cstdarg>

// Define the global flag
bool ENABLE_PRINT = true;

// Function to print variadic string arguments
void Print(const char *first, ...)
{
    if (!ENABLE_PRINT)
        return; // Discard output if printing is disabled

    // Print the first argument
    std::cout << first;

    // Initialize variadic arguments
    va_list args;
    va_start(args, first);

    const char *arg;
    while ((arg = va_arg(args, const char *)) != nullptr) // Check for nullptr to end the loop
    {
        std::cout << " " << arg;
    }

    std::cout << std::endl;

    // Clean up variadic arguments
    va_end(args);
}
