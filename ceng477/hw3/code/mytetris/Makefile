all:
	g++ -std=c++17 main.cpp Cube.cpp  TetBlock.cpp GroundBlock.cpp -o tetrisGL -g -lglfw -lpng -lpthread -lX11 -ldl -lXrandr -lGLEW -lGL -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -I. `pkg-config --cflags --libs freetype2`

clear:
	rm tetrisGL
