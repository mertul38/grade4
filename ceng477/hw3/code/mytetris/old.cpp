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

#include "Cube.h"
#include "TetBlock.cpp"
#include "GroundBlock.cpp"
// #include "TetBlock.cpp"

using namespace std;

#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <glm/glm.hpp>


// Game state
struct GameState {
    int playStep;
    int totalPlaySteps;

    GameState(){
        playStep = 0;
        totalPlaySteps = 90;
    }

    void step(GameElements* gameElements) {
        playStep += 1;
        if (playStep == totalPlaySteps){
            gameElements->allDown();
            playStep = 0;
        }

    }
};

// Game elements
struct GameElements{

    Renderer* renderer;

    TetBlock* currTetBlock;

    vector<TetBlock*> allTetBlocks;
    GroundBlock* groundBlock;

    GameElements(renderer* renderer){
        this->renderer = renderer;
        currTetBlock = new TetBlock(glm::vec3(0, 2, 0));
        allTetBlocks.push_back(currTetBlock);
        groundBlock = new GroundBlock(glm::vec3(0, -7, 0));
    }

    void drawGameElementsFaces(){
        glUniform3fv(kdLoc[0], 1, glm::value_ptr(renderer->kdCubes));
        currTetBlock->drawFaces();

        glUniform3fv(kdLoc[0], 1, glm::value_ptr(renderer->kdGround));
        groundBlock->drawFaces();
    }

    void drawGameElementsEdges(){
        currTetBlock->drawEdges();
        groundBlock->drawEdges();
    }

    void allDown(){
        currTetBlock->move(Cube::MoveDirection::Down, renderer->stableEyePos);
    }
};

class Renderer{

    public:

        // Define the shader programs
        GLuint gProgram[3];
        int gWidth = 600, gHeight = 1000;
        GLuint gVertexAttribBuffer, gIndexBuffer;

        GLint modelingMatrixLoc[2];
        GLint viewingMatrixLoc[2];
        GLint projectionMatrixLoc[2];
        GLint eyePosLoc[2];
        GLint lightPosLoc[2];
        GLint kdLoc[2];

        glm::mat4 projectionMatrix;
        glm::mat4 viewingMatrix;

        enum AngleDirection { Left, Right};

        bool isEyePosRotating = false;
        AngleDirection eyePosRotationDirection = AngleDirection::Left;
        int eyePosRotationStep = 0;
        const int totalEyePosRotationSteps = 90;  // Total steps for full rotation
        float eyeDistance = 30;

        glm::vec3 stableEyePos = glm::vec3(0, 0, eyeDistance);  // Initialize with the starting eye position
        glm::vec3 eyePos = glm::vec3(0, 0, eyeDistance);
        glm::vec3 lightPos = glm::vec3(0, 0, 7);

        glm::vec3 kdGround{0.334, 0.288, 0.635}; // this is the ground color in the demo
        glm::vec3 kdCubes{0.86, 0.11, 0.31};


        GameState gameState = GameState();
        GameElements gameElements = GameElements(this);

        bool ReadDataFromFile(
            const string& fileName, ///< [in]  Name of the shader file
            string&       data)     ///< [out] The contents of the file
        {
            fstream myfile;

            // Open the input 
            myfile.open(fileName.c_str(), std::ios::in);

            if (myfile.is_open())
            {
                string curLine;

                while (getline(myfile, curLine))
                {
                    data += curLine;
                    if (!myfile.eof())
                    {
                        data += "\n";
                    }
                }

                myfile.close();
            }
            else
            {
                return false;
            }

            return true;
        }

        GLuint createVS(const char* shaderName)
        {
            string shaderSource;

            string filename(shaderName);
            if (!ReadDataFromFile(filename, shaderSource))
            {
                cout << "Cannot find file name: " + filename << endl;
                exit(-1);
            }

            GLint length = shaderSource.length();
            const GLchar* shader = (const GLchar*) shaderSource.c_str();

            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vs, 1, &shader, &length);
            glCompileShader(vs);

            char output[1024] = {0};
            glGetShaderInfoLog(vs, 1024, &length, output);
            printf("VS compile log: %s\n", output);

            return vs;
        }

        GLuint createFS(const char* shaderName)
        {
            string shaderSource;

            string filename(shaderName);
            if (!ReadDataFromFile(filename, shaderSource))
            {
                cout << "Cannot find file name: " + filename << endl;
                exit(-1);
            }

            GLint length = shaderSource.length();
            const GLchar* shader = (const GLchar*) shaderSource.c_str();

            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fs, 1, &shader, &length);
            glCompileShader(fs);

            char output[1024] = {0};
            glGetShaderInfoLog(fs, 1024, &length, output);
            printf("FS compile log: %s\n", output);

            return fs;
        }

        void initShaders()
        {
            // Create the programs

            gProgram[0] = glCreateProgram();
            gProgram[1] = glCreateProgram();
            gProgram[2] = glCreateProgram();

            // Create the shaders for both programs

            GLuint vs1 = createVS("vert.glsl"); // for cube shading
            GLuint fs1 = createFS("frag.glsl");

            GLuint vs2 = createVS("vert2.glsl"); // for border shading
            GLuint fs2 = createFS("frag2.glsl");

            GLuint vs3 = createVS("vert_text.glsl");  // for text shading
            GLuint fs3 = createFS("frag_text.glsl");

            // Attach the shaders to the programs

            glAttachShader(gProgram[0], vs1);
            glAttachShader(gProgram[0], fs1);

            glAttachShader(gProgram[1], vs1);
            glAttachShader(gProgram[1], fs2);

            glAttachShader(gProgram[2], vs3);
            glAttachShader(gProgram[2], fs3);

            for (int i = 0; i < 3; ++i)
            {
                glLinkProgram(gProgram[i]);
                GLint status;
                glGetProgramiv(gProgram[i], GL_LINK_STATUS, &status);

                if (status != GL_TRUE)
                {
                    cout << "Program link failed: " << i << endl;
                    exit(-1);
                }
            }

            for (int i = 0; i < 2; ++i)
            {
                modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
                viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
                projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
                eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
                lightPosLoc[i] = glGetUniformLocation(gProgram[i], "lightPos");
                kdLoc[i] = glGetUniformLocation(gProgram[i], "kd");

                glUseProgram(gProgram[i]);
                glUniform3fv(eyePosLoc[i], 1, glm::value_ptr(eyePos));
                glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPos));
            }
        }

        void initVBO()
        {
            Cube::modelingMatrixLoc_face = modelingMatrixLoc[0];
            Cube::modelingMatrixLoc_edge = modelingMatrixLoc[1];
            Cube::init(gVertexAttribBuffer, gIndexBuffer);
        }

        void init() 
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            // polygon offset is used to prevent z-fighting between the cube and its borders
            glPolygonOffset(0.5, 0.5);
            glEnable(GL_POLYGON_OFFSET_FILL);

            initShaders();
            initVBO();
        }

        void drawCubeFaces() {
            GLuint program = gProgram[0];
            glUseProgram(program);

            glUniformMatrix4fv(viewingMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
            glUniformMatrix4fv(projectionMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            gameElements.drawGameElementsFaces();
            // glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdCubes));
            // curr_tetBlock->drawFaces();

            // glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdGround));
            // groundBlock->drawFaces();
        }
        void drawCubeEdges() {
            GLuint program = gProgram[1]; // Shader program for edges
            glUseProgram(program);

            glUniformMatrix4fv(viewingMatrixLoc[1], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
            glUniformMatrix4fv(projectionMatrixLoc[1], 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            // Draw the edges
            gameElements.drawGameElementsEdges();
        }

        void display() {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            drawCubeFaces();
            drawCubeEdges();

            assert(glGetError() == GL_NO_ERROR);
        }

        void reshape(GLFWwindow* window, int w, int h)
        {
            w = w < 1 ? 1 : w;
            h = h < 1 ? 1 : h;

            gWidth = w;
            gHeight = h;

            glViewport(0, 0, w, h);

            // Use perspective projection

            float fovyRad = (float) (45.0 / 180.0) * M_PI;
            projectionMatrix = glm::perspective(fovyRad, gWidth / (float) gHeight, 1.0f, 100.0f);

            // always look toward (0, 0, 0)
            viewingMatrix = glm::lookAt(eyePos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

            for (int i = 0; i < 2; ++i)
            {
                glUseProgram(gProgram[i]);
                glUniformMatrix4fv(projectionMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
                glUniformMatrix4fv(viewingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
            }
        }

        glm::vec3 rotatePos(glm::vec3& pos, float a) {
            float radian_angle = glm::radians(a); // Convert degrees to radians
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), radian_angle, glm::vec3(0, 1, 0));

            // Apply the rotation to eyePos
            glm::vec4 _pos = glm::vec4(pos, 1.0); // Convert vec3 to vec4 for matrix multiplication
            _pos = rotation * _pos; // Rotate the position

            // Normalize the position to maintain the original distance
            glm::vec3 newPos = glm::normalize(glm::vec3(_pos)) * eyeDistance;
            return newPos;
        }

        void eyePosUpdate(AngleDirection direction) {
            float a = 0.0f;
            if (direction == AngleDirection::Left) {
                a = -1.0f;
            }
            else{
                a = 1.0f;
            }
            // Create a rotation matrix around the Y-axis
            eyePos = rotatePos(eyePos, a);
        }

        void checkCollision() {
            
        }

        void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            if ((key == GLFW_KEY_A) && action == GLFW_PRESS) {
            }
            if ((key == GLFW_KEY_D) && action == GLFW_PRESS) {
            }
            if ((key == GLFW_KEY_H || key == GLFW_KEY_K) && action == GLFW_PRESS && !isEyePosRotating) {
                isEyePosRotating = true;
                eyePosRotationStep = 0;
                if (key == GLFW_KEY_H){
                    eyePosRotationDirection = AngleDirection::Left;
                    stableEyePos = rotatePos(stableEyePos, (float)-totalEyePosRotationSteps);
                }
                else{
                    eyePosRotationDirection = AngleDirection::Right;
                    stableEyePos = rotatePos(stableEyePos, (float)totalEyePosRotationSteps);
                } 
            }
        }

        void rotateEyePos() {
            if (isEyePosRotating && eyePosRotationStep < totalEyePosRotationSteps) {
                eyePosUpdate(eyePosRotationDirection);
                reshape(glfwGetCurrentContext(), gWidth, gHeight);
                eyePosRotationStep++;
            } else {
                isEyePosRotating = false;
            }
        }

        void gameStep(){
            curr_tetBlock->move(Cube::MoveDirection::Down, stableEyePos);
        }

        void mainLoop(GLFWwindow* window) {
            while (!glfwWindowShouldClose(window)) {
                if (isEyePosRotating) {
                    rotateEyePos();
                }
                gameState.step();
                
                display();
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }

        int run()   // Create Main Function For Bringing It All Together
        {
            GLFWwindow* window;
            if (!glfwInit())
            {
                exit(-1);
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(gWidth, gHeight, "tetrisGL", NULL, NULL);

            if (!window)
            {
                glfwTerminate();
                exit(-1);
            }

            glfwMakeContextCurrent(window);
            glfwSwapInterval(1);

            // Initialize GLEW to setup the OpenGL Function pointers
            if (GLEW_OK != glewInit())
            {
                std::cout << "Failed to initialize GLEW" << std::endl;
                return EXIT_FAILURE;
            }

            char rendererInfo[512] = {0};
            strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
            strcat(rendererInfo, " - ");
            strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
            glfwSetWindowTitle(window, rendererInfo);

            init();

            glfwSetWindowUserPointer(window, this);

            glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
                auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
                renderer->keyboard(win, key, scancode, action, mods);
            });

            glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int w, int h) {
                auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(win));
                renderer->reshape(win, w, h);
            });

            // glfwSetKeyCallback(window, keyboard);
            // glfwSetWindowSizeCallback(window, reshape);

            reshape(window, gWidth, gHeight); // need to call this once ourselves
            mainLoop(window); // this does not return unless the window is closed

            glfwDestroyWindow(window);
            glfwTerminate();

            return 0;
        }

};

int main(int argc, char** argv)
{
    Renderer renderer = Renderer();

    return renderer.run();
}
