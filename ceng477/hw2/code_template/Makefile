# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -Wno-deprecated-copy -Wno-sign-compare

# Output executable
TARGET = rasterizer

# Source files
SRCS = Main.cpp Print.cpp Camera.cpp Color.cpp Helpers.cpp Matrix4.cpp Mesh.cpp Rotation.cpp Scaling.cpp Scene.cpp tinyxml2.cpp Translation.cpp Triangle.cpp Vec3.cpp Vec4.cpp

# Object files (same as source files but with .o extension)
OBJS = $(SRCS:.cpp=.o)

# Default rule to build the executable
all: $(TARGET)

# Link the object files to create the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Rule to compile each .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET) *.ppm *.png

clean_out:
	rm -f *.ppm *.png
	rm -f -r ./out

