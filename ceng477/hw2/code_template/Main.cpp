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


        std::string path(xmlPath);
        std::string fileName = path.substr(path.find_last_of("/\\") + 1); // Extract file name
        std::string baseName = fileName.substr(0, fileName.find_last_of('.')); // Remove extension

        std::cout << "Base name: " << baseName << std::endl;
        scene = new Scene(xmlPath);
        scene->baseName = baseName;

        scene->render();

        delete scene;
        return 0;
    }
}