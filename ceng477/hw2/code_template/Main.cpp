#include <iostream>
#include "Scene.h"
#include "Print.h"

using namespace std;

Scene *scene;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Please run the rasterizer as:" << endl
             << "\t./rasterizer <input_file_name>" << endl;
        return 1;
    }
    else
    {
        const char *xmlPath = argv[1];

        scene = new Scene(xmlPath);
        

        scene->transformVerticesToWorld();
        cout << "vertices size: " << scene->vertices.size() << " - " << "world vertices size: " << scene->world_vertices.size() << endl; 
        Print("Transformed vertices to world space", nullptr);
        Print("Iterating over cameras", nullptr);
        for (int i = 0; i < scene->cameras.size(); i++)
        {
            Print("- Processing camera ", nullptr);
            // cout << *(scene->cameras[i]) << endl;
            // initialize image with basic values
            Print("- Initializing image", nullptr);
            scene->initializeImage(scene->cameras[i]);
        
            // do forward rendering pipeline operations
            Print("- Forward rendering pipeline", nullptr);
            scene->forwardRenderingPipeline(scene->cameras[i]);
            break;

            // generate PPM file
            scene->writeImageToPPMFile(scene->cameras[i]);

            // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
            // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
            // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
            scene->convertPPMToPNG(scene->cameras[i]->outputFilename, 0);
        }

        return 0;
    }
}