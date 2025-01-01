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
#include "Cuboid.h"
#include "TetBlock.h"
#include "GroundBlock.h"
#include "Common.h"
// #include "TetBlock.cpp"

using namespace std;

#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <glm/glm.hpp>



class Game{
    public:
        GLuint cubeVao, cuboidVao;
        // BufferOffsets bufferOffsets;
        // Define the shader programs
        GLuint gProgram[3];
        int gWidth = 600, gHeight = 1000;
        GLuint cubeVertexBuffer, cubeIndexBuffer;
        GLuint cuboidVertexBuffer, cuboidIndexBuffer; // Buffers for Cuboid
        GLuint gTextVBO;
        GLuint gTex2D;
        
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
        float eyeDistance = 35;
        glm::vec3 stableEyePos = glm::vec3(0, 0, eyeDistance);  // Initialize with the starting eye position
        glm::vec3 eyePos = glm::vec3(0, 0, eyeDistance);
        glm::vec3 lightPos = glm::vec3(0, 0, 7);

        glm::vec3 kdGround{0.78, 0.2, 0.3}; // this is the ground color in the demo
        glm::vec3 kdCubes{0.45, 0.81, 0.31};

        // --- Game ---
        vector<string> eyePositions = {"Front", "Right", "Back", "Left"};
        bool gameOver = false;
        long int score = 0;
        int topY = 8;
        int eyePositionIndex = 0;
        vector<vector<bool>> groundedPositions = vector<vector<bool>>(9, vector<bool>(9, false));
        TetBlock* currTetBlock = new TetBlock(glm::vec3(0, 7, 0));
        vector<TetBlock*> allTetBlocks = {
            currTetBlock,
        };
        float groundY = -10.0f;
        GroundBlock* groundBlock = new GroundBlock(glm::vec3(0, groundY+0.5, 0));
        int playStep = 0;
        int totalPlaySteps = 100;
        int minPlayStep = 10;
        int maxPlayStep = 100;
        int percPlayStep;
        bool pause = true;


        struct Character {
            GLuint TextureID;   // ID handle of the glyph texture
            glm::ivec2 Size;    // Size of glyph
            glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
            GLuint Advance;    // Horizontal offset to advance to next glyph
        };

        std::map<GLchar, Character> Characters;

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
                // cout << "Cannot find file name: " + filename << endl;
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
                // cout << "Cannot find file name: " + filename << endl;
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

            // Link programs
            for (int i = 0; i < 3; ++i)
            {
                glLinkProgram(gProgram[i]);
                GLint status;
                glGetProgramiv(gProgram[i], GL_LINK_STATUS, &status);
                if (status != GL_TRUE)
                {
                    // Handle linking error here, for example:
                    char log[512];
                    glGetProgramInfoLog(gProgram[i], sizeof(log), NULL, log);
                    std::cerr << "Error linking shader program: " << log << std::endl;
                    exit(-1);
                }
            }

            // Set up uniform locations for the first two shader programs
            for (int i = 0; i < 2; ++i)
            {
                modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
                viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
                projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
                eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
                lightPosLoc[i] = glGetUniformLocation(gProgram[i], "lightPos");
                kdLoc[i] = glGetUniformLocation(gProgram[i], "kd");

                // Use the program to set uniforms
                glUseProgram(gProgram[i]);
                glUniform3fv(eyePosLoc[i], 1, glm::value_ptr(eyePos));
                glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPos));

                // Additional ambient light setup
                GLint ambientIntensityLoc = glGetUniformLocation(gProgram[i], "Iamb");
                GLint ambientCoefficientLoc = glGetUniformLocation(gProgram[i], "ka");
                glm::vec3 ambientIntensity = glm::vec3(0.1f, 0.1f, 0.1f);  // Ambient light intensity
                glm::vec3 ambientCoefficient = glm::vec3(0.1f, 0.1f, 0.1f);  // Ambient reflectance coefficient

                glUniform3fv(ambientIntensityLoc, 1, glm::value_ptr(ambientIntensity));
                glUniform3fv(ambientCoefficientLoc, 1, glm::value_ptr(ambientCoefficient));
            }
        }

        
        void initFonts(int windowWidth, int windowHeight)
        {
            // Set OpenGL options
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
            glUseProgram(gProgram[2]);
            glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

            // FreeType
            FT_Library ft;
            // All functions return a value different than 0 whenever an error occurred
            if (FT_Init_FreeType(&ft))
            {
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            }

            // Load font as face
            FT_Face face;
            if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 0, &face))
            //if (FT_New_Face(ft, "/usr/share/fonts/truetype/gentium-basic/GenBkBasR.ttf", 0, &face)) // you can use different fonts
            {
                std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            }

            // Set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, 48);

            // Disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

            // Load first 128 characters of ASCII set
            for (GLubyte c = 0; c < 128; c++)
            {
                // Load character glyph 
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // Generate texture
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RED,
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        0,
                        GL_RED,
                        GL_UNSIGNED_BYTE,
                        face->glyph->bitmap.buffer
                        );
                // Set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // Now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    (GLuint) face->glyph->advance.x
                };
                Characters.insert(std::pair<GLchar, Character>(c, character));
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            // Destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            //
            // Configure VBO for texture quads
            //
            glGenBuffers(1, &gTextVBO);
            glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void initVBO()
        {
            Cube::modelingMatrixLoc_face = modelingMatrixLoc[0];
            Cube::modelingMatrixLoc_edge = modelingMatrixLoc[1];
            
            Cube::init(cubeVao, cubeVertexBuffer, cubeIndexBuffer);
            Cuboid::init(cuboidVao, cuboidVertexBuffer, cuboidIndexBuffer, 0.6f);

        }

        void init() 
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            // polygon offset is used to prevent z-fighting between the cube and its borders
            glPolygonOffset(0.5, 0.5);
            glEnable(GL_POLYGON_OFFSET_FILL);

            calculatePercPlayStep();
            initShaders();
            initVBO();
            initFonts(gWidth, gHeight);
        }

        void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
        {
            // Activate corresponding render state	
            glUseProgram(gProgram[2]);
            glUniform3f(glGetUniformLocation(gProgram[2], "textColor"), color.x, color.y, color.z);
            glActiveTexture(GL_TEXTURE0);

            // Iterate through all characters
            std::string::const_iterator c;
            for (c = text.begin(); c != text.end(); c++) 
            {
                Character ch = Characters[*c];

                GLfloat xpos = x + ch.Bearing.x * scale;
                GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

                GLfloat w = ch.Size.x * scale;
                GLfloat h = ch.Size.y * scale;

                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { xpos,     ypos + h,   0.0, 0.0 },            
                    { xpos,     ypos,       0.0, 1.0 },
                    { xpos + w, ypos,       1.0, 1.0 },

                    { xpos,     ypos + h,   0.0, 0.0 },
                    { xpos + w, ypos,       1.0, 1.0 },
                    { xpos + w, ypos + h,   1.0, 0.0 }           
                };

                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);

                // Update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

                //glBindBuffer(GL_ARRAY_BUFFER, 0);

                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

                x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }


        void drawCubeFaces() {
            GLuint program = gProgram[0];
            glUseProgram(program);

            glUniformMatrix4fv(viewingMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
            glUniformMatrix4fv(projectionMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdCubes));

            glBindVertexArray(cubeVao);
            for (int i = 0; i < allTetBlocks.size(); i++) {
                allTetBlocks[i]->drawFaces();
            }

            glBindVertexArray(cuboidVao);
            glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdGround));
            groundBlock->drawFaces();
        }
        
        void drawCubeEdges() {
            GLuint program = gProgram[1]; // Shader program for edges
            glUseProgram(program);

            glUniformMatrix4fv(viewingMatrixLoc[1], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
            glUniformMatrix4fv(projectionMatrixLoc[1], 1, GL_FALSE, glm::value_ptr(projectionMatrix));

            glBindVertexArray(cubeVao);
            for(int i = 0; i < allTetBlocks.size(); i++) {
                allTetBlocks[i]->drawEdges();
            }
            glBindVertexArray(cuboidVao);
            groundBlock->drawEdges();
            // Draw the edges
            // gameElements.drawGameElementsEdges();
        }

        void display() {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            drawCubeFaces();
            drawCubeEdges();
            renderText(eyePositions[eyePositionIndex], 10, gHeight - 50, 0.75, glm::vec3(1, 1, 0));
            renderText("Points: "+to_string(score), gWidth - 270, gHeight - 50, 0.75, glm::vec3(1, 1, 0));
            renderText("Play Speed: "+to_string(100 - percPlayStep), gWidth - 270, gHeight - 84, 0.75, glm::vec3(1, 0.6, 0));
            if(pause){
                renderText("Paused", gWidth/2 - 55, gHeight/2 - 40, 0.8, glm::vec3(0.9, 0.9, 0));
                renderText("Press S to unpause", gWidth/2 - 150, gHeight/2 - 74, 0.8, glm::vec3(0.9, 0.9, 0));
            }
            if(gameOver) {
                renderText("Game Over", gWidth/2 - 85, gHeight/2 - 60, 1, glm::vec3(1, 0, 0.2));
            }

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
                a = -3.0f;
            }
            else{
                a = 3.0f;
            }
            // Create a rotation matrix around the Y-axis
            eyePos = rotatePos(eyePos, a);
            lightPos = rotatePos(lightPos, a);

            for (int i = 0; i < 2; ++i) {  // Assuming the first two programs use the light position
                glUseProgram(gProgram[i]);
                glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPos));
            }
        }

        void rotateEyePos() {
            if (isEyePosRotating && eyePosRotationStep < totalEyePosRotationSteps) {
                eyePosUpdate(eyePosRotationDirection);
                reshape(glfwGetCurrentContext(), gWidth, gHeight);
                eyePosRotationStep+= 3;
            } else {
                isEyePosRotating = false;
            }
        }

        void calculatePercPlayStep() {
            percPlayStep = (int)(((float)(totalPlaySteps - minPlayStep) / (float)(maxPlayStep - minPlayStep)) * 100);
        }

        bool checkCollision(Cube::MoveDirection moveDirection) {
            Cube& currCenterCube = currTetBlock->getCenterCube();
            glm::vec3 currMoveVector = currCenterCube.getMoveVector(moveDirection, stableEyePos);
            glm::mat4 proposalMatrix = currCenterCube.getProposalModelMatrix(currMoveVector);
            glm::vec3 proposalPosition = Cube::modelToPosition(proposalMatrix);

            // Calculate bounds for the proposal cube
            float proposalMinX = proposalPosition.x - 1.0f;
            float proposalMaxX = proposalPosition.x + 1.0f;
            float proposalMinY = proposalPosition.y - 1.0f;
            float proposalMaxY = proposalPosition.y + 1.0f;
            float proposalMinZ = proposalPosition.z - 1.0f;
            float proposalMaxZ = proposalPosition.z + 1.0f;

            // Check if proposal cube is within the game frame bounds
            if (proposalMinX < -4.0f || proposalMaxX > 4.0f ||
                proposalMinZ < -4.0f || proposalMaxZ > 4.0f) {
                // Collision with the game frame
                return true;
            }

            // Iterate through all TetBlocks
            for (int i = 0; i < allTetBlocks.size(); i++) {
                TetBlock* tetBlock = allTetBlocks[i];
                if (tetBlock != currTetBlock) { // Exclude the current block
                    // Get the center position of the cube
                    Cube& centerCube = tetBlock->getCenterCube();
                    glm::mat4 cubeModelMatrix = centerCube.modelMatrix;
                    glm::vec3 cubePosition = Cube::modelToPosition(cubeModelMatrix);

                    // Calculate bounds for the current cube
                    float cubeMinX = cubePosition.x - 1.0f;
                    float cubeMaxX = cubePosition.x + 1.0f;
                    float cubeMinY = cubePosition.y - 1.0f;
                    float cubeMaxY = cubePosition.y + 1.0f;
                    float cubeMinZ = cubePosition.z - 1.0f;
                    float cubeMaxZ = cubePosition.z + 1.0f;

                    // Check for overlap between the two cubes' bounding boxes
                    bool overlapX = proposalMinX <= cubeMaxX && proposalMaxX >= cubeMinX;
                    bool overlapY = proposalMinY <= cubeMaxY && proposalMaxY >= cubeMinY;
                    bool overlapZ = proposalMinZ <= cubeMaxZ && proposalMaxZ >= cubeMinZ;

                    if (overlapX && overlapY && overlapZ) {
                        // Collision detected
                        // std::cout << "Collision detected with block at position: "
                        //         << cubePosition.x << ", "
                        //         << cubePosition.y << ", "
                        //         << cubePosition.z << std::endl;
                        return true;
                    }
                }
            }

            // No collision detected
            return false;
        }

        void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            if ((key == GLFW_KEY_A) && action == GLFW_PRESS) {
                bool isCollision = checkCollision(Cube::MoveDirection::Left);
                if (!isCollision) {
                    auto moveVector = currTetBlock->getCenterCube().getMoveVector(Cube::MoveDirection::Left, stableEyePos);
                    currTetBlock->move(moveVector);
                    auto currPosition = currTetBlock->getPosition();
                }
            }
            if ((key == GLFW_KEY_D) && action == GLFW_PRESS) {
                bool isCollision = checkCollision(Cube::MoveDirection::Right);
                // cout << "isCollision: " << isCollision << endl;
                if (!isCollision) {
                    auto moveVector = currTetBlock->getCenterCube().getMoveVector(Cube::MoveDirection::Right, stableEyePos);
                    currTetBlock->move(moveVector);
                    auto currPosition = currTetBlock->getPosition();
                    // cout << "currPosition: " << currPosition.x << ", " << currPosition.y << ", " << currPosition.z << endl;
                }
            }
            if ((key == GLFW_KEY_H || key == GLFW_KEY_K) && action == GLFW_PRESS && !isEyePosRotating) {
                isEyePosRotating = true;
                eyePosRotationStep = 0;
                if (key == GLFW_KEY_H){
                    eyePosRotationDirection = AngleDirection::Left;
                    eyePositionIndex = (eyePositionIndex - 1 + 4) % 4;
                    // cout << "eyePositionIndex: " << eyePositionIndex << endl;
                    stableEyePos = rotatePos(stableEyePos, (float)-totalEyePosRotationSteps);
                }
                else{
                    eyePosRotationDirection = AngleDirection::Right;
                    eyePositionIndex = (eyePositionIndex + 1 + 4) % 4;
                    // cout << "eyePositionIndex: " << eyePositionIndex << endl;
                    stableEyePos = rotatePos(stableEyePos, (float)totalEyePosRotationSteps);
                } 
            }
            if ((key == GLFW_KEY_W) && action == GLFW_PRESS) {
                if (totalPlaySteps < maxPlayStep) {
                    totalPlaySteps += 10;
                    calculatePercPlayStep();
                }
                else{
                    pause = true;       
                }
            }
            if ((key == GLFW_KEY_S) && action == GLFW_PRESS) {
                if (totalPlaySteps > minPlayStep) {
                    pause = false;
                    totalPlaySteps -= 10;
                    calculatePercPlayStep();    
                }
            }
        }

        bool checkGroundCollision(TetBlock* tetBlock, Cube::MoveDirection moveDirection) {
            Cube& centerCube = tetBlock->getCenterCube();
            glm::vec3 moveVector = centerCube.getMoveVector(moveDirection, stableEyePos);
            glm::mat4 proposalMatrix = centerCube.getProposalModelMatrix(moveVector);
            glm::vec3 proposalPosition = Cube::modelToPosition(proposalMatrix);
            
            // Calculate bounds for the proposal cube
            float proposalMinX = proposalPosition.x - 1.0f;
            float proposalMaxX = proposalPosition.x + 1.0f;
            float proposalMinY = proposalPosition.y - 1.0f;
            float proposalMaxY = proposalPosition.y + 1.0f;
            float proposalMinZ = proposalPosition.z - 1.0f;
            float proposalMaxZ = proposalPosition.z + 1.0f;

            if (proposalMinY == groundY) {
                return true;
            }
            return false;
        }

        void gameStep(){
            bool isCollision = checkCollision(Cube::MoveDirection::Down);
            bool isGroundCollision = checkGroundCollision(currTetBlock, Cube::MoveDirection::Down);
            if (!isCollision && !isGroundCollision) {
                auto moveVector = currTetBlock->getCenterCube().getMoveVector(Cube::MoveDirection::Down, stableEyePos);
                currTetBlock->move(moveVector);
            }
            else{
                if(isGroundCollision){
                    currTetBlock->landed = true;
                    currTetBlock->on_ground = true;
                    // cout << "landed on ground" << endl;
                    auto position = currTetBlock->getPosition();
                    // cout << "position: " << position.x << ", " << position.y << ", " << position.z << endl;
                    for(int i=position.x - 1; i <= position.x + 1; i++){
                        for(int j=position.z - 1; j <= position.z + 1; j++){
                            groundedPositions[i+4][j+4] = true;
                        }
                    }
                    // check if all groundedPositions are true
                    bool allTrue = true;
                    for(int i=0; i < 8; i++){
                        for(int j=0; j < 8; j++){
                            if(!groundedPositions[i][j]){
                                allTrue = false;
                                break;
                            }
                        }
                        if(!allTrue){
                            break;
                        }
                    }
                    if(allTrue){
                        for(int i=0; i < 8; i++){
                            for(int j=0; j < 8; j++){
                                groundedPositions[i][j] = false;
                            }
                        }
                        // cout << "allTrue" << endl;
                        score += 243;
                        for (int i=0; i<allTetBlocks.size(); i++){
                            auto tetBlock = allTetBlocks[i];
                            if (tetBlock->on_ground){
                                allTetBlocks.erase(allTetBlocks.begin() + i);
                                i--;
                            }
                            else
                            {
                                for (int j=0; j<3; j++){
                                    auto moveVector = tetBlock->getCenterCube().getMoveVector(Cube::MoveDirection::Down, stableEyePos);
                                    tetBlock->move(moveVector);
                                }
                                bool isGroundCollision2 = checkGroundCollision(tetBlock, Cube::MoveDirection::Down);
                                if (isGroundCollision2){
                                    tetBlock->landed = true;
                                    tetBlock->on_ground = true;
                                    // cout << "landed on ground" << endl;
                                    auto position = tetBlock->getPosition();
                                    // cout << "position: " << position.x << ", " << position.y << ", " << position.z << endl;
                                    for(int i=position.x - 1; i <= position.x + 1; i++){
                                        for(int j=position.z - 1; j <= position.z + 1; j++){
                                            groundedPositions[i+4][j+4] = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else{
                    currTetBlock->landed = false;
                    // cout << "landed on block" << endl;
                    auto position = currTetBlock->getPosition();
                    if ((position.y + 1) >= topY) {
                        // cout << "GAME OVER" << endl;
                        gameOver = true;
                        return;
                    }
                }
                currTetBlock = new TetBlock(glm::vec3(0, topY, 0));
                allTetBlocks.push_back(currTetBlock);
            }

        }

        void mainLoop(GLFWwindow* window) {
            while (!glfwWindowShouldClose(window)) {
                if (isEyePosRotating) {
                    rotateEyePos();
                }
                
                if (!gameOver && !pause) {
                    if (playStep < totalPlaySteps) {
                        playStep++;
                    }
                    else {
                        gameStep();
                        playStep = 0;
                    }                    
                }
                
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
                // std::cout << "Failed to initialize GLEW" << std::endl;
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
                auto* renderer = static_cast<Game*>(glfwGetWindowUserPointer(win));
                renderer->keyboard(win, key, scancode, action, mods);
            });

            glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int w, int h) {
                auto* renderer = static_cast<Game*>(glfwGetWindowUserPointer(win));
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
