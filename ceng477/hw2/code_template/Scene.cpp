#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "Print.h"
#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"

#include <cassert>

using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}


/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "./magick " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}


/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
    // Step 1: Transform vertices to camera space
    transformVerticesToCamera(camera);
	cout << "world vertices size: " << world_vertices.size() << " - " << "camera vertices size: " << camera->camera_vertices.size() << endl;
    Print("-- Vertices transformed to camera space.", nullptr);
    // Step 2: Project vertices to clip space
    projectVertices(camera);
	cout << "camera vertices size: " << camera->camera_vertices.size() << " - " << "projected vertices size: " << camera->projected_vertices.size() << endl;
    Print("-- Vertices projected to clip space.", nullptr);
    // Step 3: Process each mesh
    for (Mesh *mesh : meshes)
    {
        Print("--- Processing mesh ", nullptr);
        cout << mesh->meshId << endl;

		backfaceCulling(camera, mesh);
        // Replace mesh triangles with visible ones for processing
        cout << "triangles size: " << mesh->triangles.size() << " - " << "visible triangles size: " << mesh->visible_triangles.size() << endl;
		break;
        // Step 4: Clip triangles (if necessary) and map to viewport
        clipTriangles(camera, mesh); // Use only visible triangles
        Print("--- Triangles clipped.", nullptr);

        mapToViewport(camera, mesh);
        Print("--- Triangles mapped to viewport.", nullptr);

        // Step 5: Rasterize triangles
        rasterizeTriangles(camera, mesh);
        Print("--- Triangles rasterized.", nullptr);
    }
}

void Scene::transformVerticesToWorld()
{
	for (Mesh* mesh : meshes){
		Matrix4 composedTransformationMatrix = getIdentityMatrix();
		for(int i = 0; i < mesh->transformationTypes.size(); i++){
			char transformationType = mesh->transformationTypes[i];
			int transformationID = mesh->transformationIds[i];
			Matrix4* transformationMatrix = nullptr;
			switch (transformationType)
			{
			case 'r':{
				Rotation* rotation = this->rotations[transformationID-1];
				Matrix4 m = createRotationMatrix(rotation);
				transformationMatrix = &m;
				break;
			}
			case 's':{
				Scaling* scaling = this->scalings[transformationID-1];
				Matrix4 m = createScalingMatrix(scaling);
				transformationMatrix = &m;
				break;
			}
			case 't':{
				Translation* translation = this->translations[transformationID-1];
				Matrix4 m = createTranslationMatrix(translation);
				transformationMatrix = &m;
				break;
			}
			default:
				break;
			}

			composedTransformationMatrix = multiplyMatrixWithMatrix(*transformationMatrix, composedTransformationMatrix);
		}

		this->world_vertices.resize(this->vertices.size(), nullptr);


		for (int i = 0; i < mesh->triangles.size(); i++)
		{
			Triangle& triangle = mesh->triangles[i];
			for(int vertexId : triangle.vertexIds){
				if(world_vertices[vertexId-1] != nullptr){
					continue;
				}
				
				Vec3* vertex = vertices[vertexId-1];
				Vec4 extended_vertex = Vec4(vertex->x, vertex->y, vertex->z, 1, vertex->colorId);
				Vec4 transformed_vertex = multiplyMatrixWithVec4(composedTransformationMatrix, extended_vertex);
			
				world_vertices[vertexId-1] = new Vec3(transformed_vertex.x, transformed_vertex.y, transformed_vertex.z, vertex->colorId);

			}
		}

	}

}

// Rendering pipeline
void Scene::transformVerticesToCamera(Camera *camera)
{
    // Ensure camera_vertices is sized correctly
    camera->camera_vertices.resize(world_vertices.size(), nullptr);

    // Extract camera basis vectors and position
    Vec3 &u = camera->u; // Camera's right vector
    Vec3 &v = camera->v; // Camera's up vector
    Vec3 &w = camera->w; // Camera's forward vector (negative gaze)

    // Construct the view matrix
    double values[4][4] = {
        {u.x, u.y, u.z, -dotProductVec3(u, camera->position)},
        {v.x, v.y, v.z, -dotProductVec3(v, camera->position)},
        {w.x, w.y, w.z, -dotProductVec3(w, camera->position)},
        {0,   0,   0,   1}};

    Matrix4 viewMatrix(values);


    // Transform each world vertex to camera space
    for (int i = 0; i < world_vertices.size(); i++)
    {
        Vec3 *worldVertex = world_vertices[i];
        Vec4 extendedVertex = Vec4(worldVertex->x, worldVertex->y, worldVertex->z, 1);
        Vec4 transformedVertex = multiplyMatrixWithVec4(viewMatrix, extendedVertex);

        // Store the transformed vertex in camera_vertices
        camera->camera_vertices[i] = new Vec3(transformedVertex.x, transformedVertex.y, transformedVertex.z);
    }

}

void Scene::projectVertices(Camera *camera)
{
    camera->projected_vertices.resize(camera->camera_vertices.size(), nullptr);

    double &right = camera->right;
    double &left = camera->left;
    double &top = camera->top;
    double &bottom = camera->bottom;
    double &near = camera->near;
    double &far = camera->far;
    Matrix4 projectionMatrix;

    if (camera->projectionType == ORTOGRAPHIC_PROJECTION)
    {
        double values[4][4] = {
            {2 / (right - left), 0, 0, -(right + left) / (right - left)},
            {0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom)},
            {0, 0, -2 / (far - near), -(far + near) / (far - near)},
            {0, 0, 0, 1}};
        projectionMatrix = Matrix4(values);
    }
    else if (camera->projectionType == PERSPECTIVE_PROJECTION)
    {
        double values[4][4] = {
            {2 * near / (right - left), 0, (right + left) / (right - left), 0},
            {0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0},
            {0, 0, -(far + near) / (far - near), -(2 * far * near) / (far - near)},
            {0, 0, -1, 0}};
        projectionMatrix = Matrix4(values);
    }

    for (int i = 0; i < camera->projected_vertices.size(); i++)
    {
        Vec3 *cameraVertex = camera->camera_vertices[i];
        Vec4 extendedVertex = Vec4(cameraVertex->x, cameraVertex->y, cameraVertex->z, 1);
        Vec4 projectedVertex = multiplyMatrixWithVec4(projectionMatrix, extendedVertex);

        // Perspective divide only for perspective projection
        if (camera->projectionType == PERSPECTIVE_PROJECTION)
        {
            if (std::abs(projectedVertex.t) > EPSILON) // Check if w is non-zero (using EPSILON for floating-point safety)
            {
                projectedVertex.x /= projectedVertex.t;
                projectedVertex.y /= projectedVertex.t;
                projectedVertex.z /= projectedVertex.t;
            }
            else
            {
                // Handle edge case where w is zero (optional logging or fallback)
                assert(false && "Perspective divide failed: w is zero."); // Debugging aid
                // Alternatively, you can skip this vertex or assign a fallback value.
                projectedVertex.x = 0;
                projectedVertex.y = 0;
                projectedVertex.z = 0;
            }
        }

        // Store the result
        camera->projected_vertices[i] = new Vec3(projectedVertex.x, projectedVertex.y, projectedVertex.z);
    }
}

void Scene::backfaceCulling(Camera *camera, Mesh *mesh) {
	// Collect only visible triangles after backface culling
	mesh->visible_triangles.resize(mesh->triangles.size(), nullptr);
	cout << "triangles size: " << mesh->triangles.size() << " - " << "visible triangles size: " << mesh->visible_triangles.size() << endl;


	for (int i = 0; i < mesh->triangles.size(); i++)
	{
		Triangle &triangle = mesh->triangles[i];
		Vec3 *v1 = camera->projected_vertices[triangle.vertexIds[0] - 1];
		Vec3 *v2 = camera->projected_vertices[triangle.vertexIds[1] - 1];
		Vec3 *v3 = camera->projected_vertices[triangle.vertexIds[2] - 1];

		// Perform backface culling
		if (cullingEnabled && isBackface(*v1, *v2, *v3, camera->position))
		{
			continue; // Skip back-facing triangle
		}

		// Add to the list of visible triangles
		mesh->visible_triangles[i] = &triangle;
	}
}

void Scene::clipTriangles(Camera *camera, Mesh *mesh)
{
    clipped_vertices.clear();
    for (Triangle &triangle : mesh->triangles)
    {
        Vec3 *v1 = camera->projected_vertices[triangle.vertexIds[0] - 1];
        Vec3 *v2 = camera->projected_vertices[triangle.vertexIds[1] - 1];
        Vec3 *v3 = camera->projected_vertices[triangle.vertexIds[2] - 1];

        // Perform clipping logic here
        std::vector<int> clippedVertexIds;
        bool edge1Clipped = clipLine(*v1, *v2, triangle.vertexIds[0], triangle.vertexIds[1], clipped_vertices, clippedVertexIds);
        bool edge2Clipped = clipLine(*v2, *v3, triangle.vertexIds[1], triangle.vertexIds[2], clipped_vertices, clippedVertexIds);
        bool edge3Clipped = clipLine(*v3, *v1, triangle.vertexIds[2], triangle.vertexIds[0], clipped_vertices, clippedVertexIds);

        // Create new triangles from clipped vertices
        if (edge1Clipped && edge2Clipped && edge3Clipped)
        {
            for (size_t i = 1; i < clippedVertexIds.size() - 1; i++)
            {
                mesh->clipped_triangles.push_back(
                    new Triangle(clippedVertexIds[0], clippedVertexIds[i], clippedVertexIds[i + 1]));
            }
        }
    }
}

void Scene::mapToViewport( Camera *camera, Mesh *mesh)
{
	cout << "clippd vertices size: " << clipped_vertices.size() << endl;
	new_viewport_vertices.resize(clipped_vertices.size(), nullptr);
    // Iterate over clipped vertices
    for (Vec3 *vertex : clipped_vertices)
    {
        if (vertex == nullptr)
            continue;

        // Map NDC coordinates to screen space
        double xScreen = (vertex->x + 1) * 0.5 * camera->horRes;
        double yScreen = (vertex->y + 1) * 0.5 * camera->verRes;
        double zBuffer = (vertex->z + 1) * 0.5; // Map z to [0, 1] for depth

        // Create a new vertex with screen coordinates and mapped z-buffer value
        Vec3 *newVertex = new Vec3(xScreen, yScreen, zBuffer, vertex->colorId);

        // Add the new vertex to the list
		cout << "adding to new viewport, new size: " << new_viewport_vertices.size() << endl;
        new_viewport_vertices.push_back(newVertex);
    }
}

void Scene::rasterizeTriangles(Camera *camera, Mesh *mesh)
{
    int horRes = camera->horRes;
    int verRes = camera->verRes;
    std::vector<std::vector<double>> depthBuffer(verRes, std::vector<double>(horRes, -1e9)); // Correct indexing

    for (Triangle *triangle : mesh->clipped_triangles)
    {
		cout << "Triangle: " << triangle->vertexIds[0] << ", " << triangle->vertexIds[1] << ", " << triangle->vertexIds[2] << endl;
		cout << "new viewport vertices size: " << new_viewport_vertices.size() << endl;
        Vec3 *v1 = new_viewport_vertices[triangle->vertexIds[0] - 1];
        Vec3 *v2 = new_viewport_vertices[triangle->vertexIds[1] - 1];
        Vec3 *v3 = new_viewport_vertices[triangle->vertexIds[2] - 1];

        if (mesh->type == WIREFRAME_MESH)
        {
			cout << "Wireframe" << endl;
			cout << "Draw line from " << v1->x << ", " << v1->y << " to " << v2->x << ", " << v2->y << endl;
            drawLine(v1, v2, depthBuffer, camera);
			cout << "Draw line from " << v2->x << ", " << v2->y << " to " << v3->x << ", " << v3->y << endl;
            drawLine(v2, v3, depthBuffer, camera);
			cout << "Draw line from " << v3->x << ", " << v3->y << " to " << v1->x << ", " << v1->y << endl;
            drawLine(v3, v1, depthBuffer, camera);
        }
        else if (mesh->type == SOLID_MESH)
        {
            fillTriangle(v1, v2, v3, depthBuffer, camera);
        }
    }
}


// helpers
bool Scene::isBackface(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3, const Vec3 &cameraPosition) {
    // Calculate the edges of the triangle
    Vec3 edge1 = subtractVec3(v2, v1);
    Vec3 edge2 = subtractVec3(v3, v1);
    
    // Compute the normal using cross product
    Vec3 normal = crossProductVec3(edge1, edge2);
    
    // Vector from camera to vertex v1
    Vec3 viewVector = subtractVec3(v1, cameraPosition);
    
    // Dot product to determine if the triangle is facing the camera
    double dot = dotProductVec3(normal, viewVector);
    
    // If dot product is positive, the triangle is facing away from the camera (backface)
    return dot > 0;
}

int computeOutcode(const Vec3 &vertex)
{
    int code = 0;

    if (vertex.x < -1) code |= 1;  // Left
    if (vertex.x > 1) code |= 2;   // Right
    if (vertex.y < -1) code |= 4;  // Bottom
    if (vertex.y > 1) code |= 8;   // Top
    if (vertex.z < -1) code |= 16; // Near
    if (vertex.z > 1) code |= 32;  // Far

    return code;
}

bool clipLine(Vec3 &start, Vec3 &end, int startId, int endId, std::vector<Vec3 *> &clippedVertices, std::vector<int> &clippedVertexIds)
{
    int outcode1 = computeOutcode(start);
    int outcode2 = computeOutcode(end);

    while (true)
    {
        if ((outcode1 | outcode2) == 0)
        {
            // Both endpoints are inside the clipping region
            clippedVertexIds.push_back(startId);
            clippedVertexIds.push_back(endId);
            return true;
        }
        else if ((outcode1 & outcode2) != 0)
        {
            // Line is completely outside
            return false;
        }
        else
        {
            // Line is partially inside
            int outcodeOut = outcode1 ? outcode1 : outcode2;

            Vec3 intersection;
            if (outcodeOut & 1) // Left
            {
                intersection.x = -1;
                intersection.y = start.y + (end.y - start.y) * (-1 - start.x) / (end.x - start.x);
                intersection.z = start.z + (end.z - start.z) * (-1 - start.x) / (end.x - start.x);
            }
            else if (outcodeOut & 2) // Right
            {
                intersection.x = 1;
                intersection.y = start.y + (end.y - start.y) * (1 - start.x) / (end.x - start.x);
                intersection.z = start.z + (end.z - start.z) * (1 - start.x) / (end.x - start.x);
            }
            else if (outcodeOut & 4) // Bottom
            {
                intersection.y = -1;
                intersection.x = start.x + (end.x - start.x) * (-1 - start.y) / (end.y - start.y);
                intersection.z = start.z + (end.z - start.z) * (-1 - start.y) / (end.y - start.y);
            }
            else if (outcodeOut & 8) // Top
            {
                intersection.y = 1;
                intersection.x = start.x + (end.x - start.x) * (1 - start.y) / (end.y - start.y);
                intersection.z = start.z + (end.z - start.z) * (1 - start.y) / (end.y - start.y);
            }
            else if (outcodeOut & 16) // Near
            {
                intersection.z = -1;
                intersection.x = start.x + (end.x - start.x) * (-1 - start.z) / (end.z - start.z);
                intersection.y = start.y + (end.y - start.y) * (-1 - start.z) / (end.z - start.z);
            }
            else if (outcodeOut & 32) // Far
            {
                intersection.z = 1;
                intersection.x = start.x + (end.x - start.x) * (1 - start.z) / (end.z - start.z);
                intersection.y = start.y + (end.y - start.y) * (1 - start.z) / (end.z - start.z);
            }

            // Add the new intersection vertex to clippedVertices
            Vec3 *newVertex = new Vec3(intersection.x, intersection.y, intersection.z);
            clippedVertices.push_back(newVertex);
            int newVertexId = clippedVertices.size(); // Assign a new vertex ID

            // Replace the outside vertex with the intersection point
            if (outcodeOut == outcode1)
            {
                start = *newVertex;
                startId = newVertexId;
                outcode1 = computeOutcode(start);
            }
            else
            {
                end = *newVertex;
                endId = newVertexId;
                outcode2 = computeOutcode(end);
            }
        }
    }
}

void Scene::drawLine(Vec3 *v1, Vec3 *v2, std::vector<std::vector<double>> &depthBuffer, Camera *camera)
{
    int x1 = static_cast<int>(v1->x);
    int y1 = static_cast<int>(v1->y);
    int x2 = static_cast<int>(v2->x);
    int y2 = static_cast<int>(v2->y);
    double z1 = v1->z;
    double z2 = v2->z;

    bool steep = abs(y2 - y1) > abs(x2 - x1);

    if (steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
        std::swap(z1, z2);
    }

    int dx = x2 - x1;
    int dy = abs(y2 - y1);
    int error = dx / 2;
    int yStep = (y1 < y2) ? 1 : -1;

    int y = y1;
    for (int x = x1; x <= x2; x++)
    {
        double t = static_cast<double>(x - x1) / dx;
        double z = z1 + t * (z2 - z1);

        if (steep)
        {
            updateDepthBuffer(y, x, z, *colorsOfVertices[v1->colorId - 1], depthBuffer, camera);
        }
        else
        {
            updateDepthBuffer(x, y, z, *colorsOfVertices[v1->colorId - 1], depthBuffer, camera);
        }

        error -= dy;
        if (error < 0)
        {
            y += yStep;
            error += dx;
        }
    }
}

// Helper to rasterize a triangle using barycentric coordinates
void Scene::fillTriangle(Vec3 *v1, Vec3 *v2, Vec3 *v3, std::vector<std::vector<double>> &depthBuffer, Camera *camera)
{
    int minX = std::max(0, static_cast<int>(std::min({v1->x, v2->x, v3->x})));
    int maxX = std::min(camera->horRes - 1, static_cast<int>(std::max({v1->x, v2->x, v3->x})));
    int minY = std::max(0, static_cast<int>(std::min({v1->y, v2->y, v3->y})));
    int maxY = std::min(camera->verRes - 1, static_cast<int>(std::max({v1->y, v2->y, v3->y})));

    double area = edgeFunction(v1, v2, v3);

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            Vec3 pixel(static_cast<double>(x), static_cast<double>(y), 0);
            double w1 = edgeFunction(v2, v3, &pixel) / area;
            double w2 = edgeFunction(v3, v1, &pixel) / area;
            double w3 = edgeFunction(v1, v2, &pixel) / area;

            if (w1 >= 0 && w2 >= 0 && w3 >= 0)
            {
                double z = w1 * v1->z + w2 * v2->z + w3 * v3->z;
                Color color = interpolateColor(w1, w2, w3, v1->colorId, v2->colorId, v3->colorId);

                updateDepthBuffer(x, y, z, color, depthBuffer, camera);
            }
        }
    }
}

// Edge function to compute the area of a sub-triangle
double Scene::edgeFunction(Vec3 *v1, Vec3 *v2, Vec3 *p)
{
    return (p->x - v1->x) * (v2->y - v1->y) - (p->y - v1->y) * (v2->x - v1->x);
}

// Interpolate color using barycentric weights
Color Scene::interpolateColor(double w1, double w2, double w3, int colorId1, int colorId2, int colorId3)
{
    Color *c1 = colorsOfVertices[colorId1 - 1];
    Color *c2 = colorsOfVertices[colorId2 - 1];
    Color *c3 = colorsOfVertices[colorId3 - 1];

    Color result;
    result.r = w1 * c1->r + w2 * c2->r + w3 * c3->r;
    result.g = w1 * c1->g + w2 * c2->g + w3 * c3->g;
    result.b = w1 * c1->b + w2 * c2->b + w3 * c3->b;

    return result;
}

// Update the depth buffer and draw pixel
void Scene::updateDepthBuffer(int x, int y, float depth, Color color, std::vector<std::vector<double>> &depthBuffer, Camera *camera)
{
    if (depth > depthBuffer[y][x]) // Correct indexing for depthBuffer
    {
        depthBuffer[y][x] = depth;
        image[x][y] = color; // Keep this indexing for the image
    }
}
