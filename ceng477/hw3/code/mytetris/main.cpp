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

#include <chrono>
#include <thread>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include "Game.cpp"
// #include "TetBlock.cpp"

using namespace std;

#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <glm/glm.hpp>

int main(int argc, char** argv)
{
    Game game = Game();
    game.run();

    return 0;
}
