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
#include <limits>

#include "Print.h"
#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include <unordered_map>

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

	zBuffer = std::vector<std::vector<double>>(camera->horRes, std::vector<double>(camera->verRes, std::numeric_limits<double>::infinity()));
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

void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		cout << "convert " << ppmFileName << " " << ppmFileName << ".png" << endl;
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
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


void Scene::myWriteImageToPPMFile(Camera *camera)
{
	ofstream fout;

	// write image to out/outputFilename

	string outPath = "./out/";
	if(cullingEnabled){
		outPath += "culling/";
	}
	else{
		outPath += "no_culling/";
	}
	outPath += baseName + "/" + camera->outputFilename;
	

	fout.open(outPath.c_str());

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

void Scene::myConvertPPMToPNG(string ppmFileName, int osType)
{
	string command;
	
	string inPath = "./out/";
	if(cullingEnabled){
		inPath += "culling/";
	}
	else{
		inPath += "no_culling/";
	}
	inPath += baseName + "/" + ppmFileName;
	
	string outPath = inPath + ".png";
	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + inPath + " " + outPath;
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

void Scene::transform_vertices_to_world(Mesh& mesh) 
{
	std::vector<Triangle*> world_triangles;
    std::vector<Vec3*> world_vertices;
    std::unordered_map<int, int> vertex_map; // Maps vertexId -> mesh_vertex_index

    Matrix4 composedTransformationMatrix = getIdentityMatrix();
    for (int i = 0; i < mesh.transformationTypes.size(); i++) {
        char transformationType = mesh.transformationTypes[i];
        int transformationID = mesh.transformationIds[i];
        Matrix4* transformationMatrix = nullptr;
        switch (transformationType) {
        case 'r': {
            Rotation* rotation = this->rotations[transformationID - 1];
            Matrix4 m = createRotationMatrix(rotation);
            transformationMatrix = &m;
            break;
        }
        case 's': {
            Scaling* scaling = this->scalings[transformationID - 1];
            Matrix4 m = createScalingMatrix(scaling);
            transformationMatrix = &m;
            break;
        }
        case 't': {
            Translation* translation = this->translations[transformationID - 1];
            Matrix4 m = createTranslationMatrix(translation);
            transformationMatrix = &m;
            break;
        }
        default:
            break;
        }

        composedTransformationMatrix = multiplyMatrixWithMatrix(*transformationMatrix, composedTransformationMatrix);
    }

    int world_vertex_index = 0;
    for (int i = 0; i < mesh.triangles.size(); i++) {
        const Triangle& triangle = mesh.triangles[i];
        int new_vertices[3] = {};
        for (int j = 0; j < 3; j++) {
            int vertexId = triangle.vertexIds[j];

            // Check if the vertex has already been processed
            if (vertex_map.find(vertexId) != vertex_map.end()) {
                // Vertex already processed, use the existing index
                new_vertices[j] = vertex_map[vertexId];
            } 
			else {
                // Transform the vertex
                Vec3* vertex = vertices[vertexId - 1];
                Vec4 extended_vertex = Vec4(vertex->x, vertex->y, vertex->z, 1);
                Vec4 transformed_vertex = multiplyMatrixWithVec4(composedTransformationMatrix, extended_vertex);

                // Add the transformed vertex to the list
				world_vertices.push_back(
                    new Vec3(
                        transformed_vertex.x,
                        transformed_vertex.y,
                        transformed_vertex.z,
                        vertex->colorId));

                // Map the original vertexId to the new index
                vertex_map[vertexId] = world_vertex_index;
                new_vertices[j] = world_vertex_index;
				world_vertex_index++;
            }
        }

        // Create and store the new triangle
        Triangle* world_triangle = new Triangle(new_vertices[0], new_vertices[1], new_vertices[2]);
        world_triangles.push_back(world_triangle);
    }

	mesh.world_vertices = world_vertices;
	mesh.world_triangles = world_triangles;
}

void Scene::transform_vertices_to_camera(Camera& camera, Mesh& mesh)
{

	std::vector<Vec3*> camera_vertices;

    // Extract camera basis vectors and position
    Vec3 &u = camera.u; // Camera's right vector
    Vec3 &v = camera.v; // Camera's up vector
    Vec3 &w = camera.w; // Camera's forward vector (negative gaze)


    // Construct the view matrix
    double values[4][4] = {
        {u.x, u.y, u.z, -dotProductVec3(u, camera.position)},
        {v.x, v.y, v.z, -dotProductVec3(v, camera.position)},
        {w.x, w.y, w.z, -dotProductVec3(w, camera.position)},
        {0,   0,   0,   1}};

    Matrix4 viewMatrix(values);

    // Transform each world vertex to camera space
    for (int i = 0; i < mesh.world_vertices.size(); i++)
    {
        Vec3 *worldVertex = mesh.world_vertices[i];
        Vec4 extendedVertex = Vec4(worldVertex->x, worldVertex->y, worldVertex->z, 1);
        Vec4 transformedVertex = multiplyMatrixWithVec4(viewMatrix, extendedVertex);

        // Store the transformed vertex in camera_vertices
        camera_vertices.push_back(
			new Vec3(
				transformedVertex.x,
				transformedVertex.y,
				transformedVertex.z,
				worldVertex->colorId
				));
    }

	mesh.camera_vertices = camera_vertices;

}

void Scene::project_camera_vertices(Camera& camera, Mesh& mesh)
{

	std::vector<Vec4* > projected_vertices;

    double &right = camera.right;
    double &left = camera.left;
    double &top = camera.top;
    double &bottom = camera.bottom;
    double &near = camera.near;
    double &far = camera.far;
    Matrix4 projectionMatrix;

    if (camera.projectionType == ORTOGRAPHIC_PROJECTION)
    {
		double values[4][4] = {
			{2 / (right - left), 0, 0, -(right + left) / (right - left)},
			{0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom)},
			{0, 0, -2 / (far - near), -(far + near) / (far - near)},
			{0, 0, 0, 1}};
		projectionMatrix = Matrix4(values);
    }
    else if (camera.projectionType == PERSPECTIVE_PROJECTION)
    {
       	double values[4][4] = {
            {2 * near / (right - left), 0, (right + left) / (right - left), 0},
            {0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0},
            {0, 0, -(far + near) / (far - near), -(2 * far * near) / (far - near)},
            {0, 0, -1, 0}};

		projectionMatrix = Matrix4(values);
    }



    for (int i = 0; i < mesh.camera_vertices.size(); i++)
    {
        Vec3& cameraVertex = *mesh.camera_vertices[i];
        Vec4 extendedVertex = Vec4(cameraVertex.x, cameraVertex.y, cameraVertex.z, 1);
        Vec4 projectedVertex = multiplyMatrixWithVec4(projectionMatrix, extendedVertex);
		projectedVertex.colorId = cameraVertex.colorId;
        projected_vertices.push_back(
			new Vec4(projectedVertex)
			);
        
    }

	mesh.projected_vertices = projected_vertices;
}

void Scene::perspective_divide(Mesh& mesh) {

	std::vector<Vec3*> perspected_vertices;

    for (int i = 0; i < mesh.clipped_vertices.size(); i++) {
        Vec4& v = *mesh.clipped_vertices[i];
		perspected_vertices.push_back(
			new Vec3(
				v.x / v.t,
				v.y / v.t,
				v.z / v.t,
				v.colorId
			));
    }

	mesh.perspected_vertices = perspected_vertices;
}

void Scene::viewport_transform(Camera& camera, Mesh& mesh) {
    // Ensure the viewport-transformed vertices vector is resized
    std::vector<Vec3 *> viewport_vertices;

    // Retrieve screen dimensions from the camera
    double nx = static_cast<double>(camera.horRes);
    double ny = static_cast<double>(camera.verRes);

    // Define the viewport transformation matrix M_vp
    double values[4][4] = {
        {nx / 2, 0, 0, (nx - 1) / 2},
        {0, ny / 2, 0, (ny - 1) / 2},
        {0, 0, 1.0 / 2, 1.0 / 2},
        {0, 0, 0, 1}
    };
    Matrix4 viewportMatrix(values);

    // Apply the viewport transformation to each vertex
    for (int i = 0; i < mesh.perspected_vertices.size(); i++) {
        Vec3& v = *mesh.perspected_vertices[i];
        
        // Extend the 3D vertex to 4D for matrix multiplication
        Vec4 extendedVertex(v.x, v.y, v.z, 1);

        // Apply the viewport transformation
        Vec4 transformedVertex = multiplyMatrixWithVec4(viewportMatrix, extendedVertex);

        // Store the transformed vertex in the viewport_vertices vector
        viewport_vertices.push_back(
			new Vec3(
				transformedVertex.x,
				transformedVertex.y,
				transformedVertex.z,
				v.colorId));
    }

	mesh.viewport_vertices = viewport_vertices;
}

void Scene::backface_culling(Camera& camera, Mesh& mesh) {

	for(int i = 0; i < mesh.world_triangles.size(); i++) {
		Triangle& triangle = *mesh.world_triangles[i];

		int v1id = triangle.vertexIds[0];
		int v2id = triangle.vertexIds[1];
		int v3id = triangle.vertexIds[2];

		Vec3* v1 = mesh.camera_vertices[v1id];
		Vec3* v2 = mesh.camera_vertices[v2id];
		Vec3* v3 = mesh.camera_vertices[v3id];

		Vec3 v1v2 = normalizeVec3(*v2 - *v1);
		Vec3 v1v3 = normalizeVec3(*v3 - *v1);

		Vec3 normal = crossProductVec3(v1v3, v1v2);
		Vec3 camera_v = normalizeVec3(*v1);

		double dot = dotProductVec3(normal, camera_v);

		if (dot < EPSILON) {
			mesh.culled_triangles.push_back(false);
		}
		else {
			mesh.culled_triangles.push_back(true);
		}

	}
}

void Scene::rasterize(Camera& camera, Mesh& mesh) {
    // Initialize the Z-buffer
	std::vector<Triangle*> triangles = mesh.type == WIREFRAME_MESH ? mesh.clipped_triangles : mesh.world_triangles;

	for (int i = 0; i < mesh.clipped_triangles.size(); i++) {
		Triangle& triangle = *mesh.clipped_triangles[i];
		if (cullingEnabled && !mesh.culled_triangles[i]) {
			continue;
		}
		int v1id = triangle.vertexIds[0];
		int v2id = triangle.vertexIds[1];
		int v3id = triangle.vertexIds[2];

		// Get vertices in viewport coordinates
		Vec3* v1 = mesh.viewport_vertices[v1id];
		Vec3* v2 = mesh.viewport_vertices[v2id];
		Vec3* v3 = mesh.viewport_vertices[v3id];

		// Get vertex colors
		Color* c1 = colorsOfVertices[v1->colorId - 1];
		Color* c2 = colorsOfVertices[v2->colorId - 1];
		Color* c3 = colorsOfVertices[v3->colorId - 1];

		if (mesh.type == WIREFRAME_MESH) {
			// Draw edges of the triangle with interpolated colors
			drawLineWithZBuffer(round(v1->x), round(v1->y), v1->z, round(v2->x), round(v2->y), v2->z, c1, c2);
			drawLineWithZBuffer(round(v2->x), round(v2->y), v2->z, round(v3->x), round(v3->y), v3->z, c2, c3);
			drawLineWithZBuffer(round(v3->x), round(v3->y), v3->z, round(v1->x), round(v1->y), v1->z, c3, c1);
		}
		else if(mesh.type == SOLID_MESH){
			// Compute bounding box of the triangle
			int minX = std::max(0, static_cast<int>(std::floor(std::min({v1->x, v2->x, v3->x}))));
			int maxX = std::min(camera.horRes - 1, static_cast<int>(std::ceil(std::max({v1->x, v2->x, v3->x}))));
			int minY = std::max(0, static_cast<int>(std::floor(std::min({v1->y, v2->y, v3->y}))));
			int maxY = std::min(camera.verRes - 1, static_cast<int>(std::ceil(std::max({v1->y, v2->y, v3->y}))));

			// Precompute areas for barycentric calculation
			double area = (v2->x - v1->x) * (v3->y - v1->y) - (v3->x - v1->x) * (v2->y - v1->y);

			// Iterate over pixels in the bounding box
			for (int x = minX; x <= maxX; x++) {
				for (int y = minY; y <= maxY; y++) {
					// Calculate barycentric coordinates
					double w1 = ((v2->x - x) * (v3->y - y) - (v3->x - x) * (v2->y - y)) / area;
					double w2 = ((v3->x - x) * (v1->y - y) - (v1->x - x) * (v3->y - y)) / area;
					double w3 = 1.0 - w1 - w2;

					// Check if the pixel is inside the triangle
					if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
						// Interpolate depth and color
						double z = w1 * v1->z + w2 * v2->z + w3 * v3->z;
						Color interpolatedColor = (*c1) * w1 + (*c2) * w2 + (*c3) * w3;

						// Z-buffer test
						if (z < zBuffer[x][y]) {
							zBuffer[x][y] = z;
							image[x][y] = interpolatedColor;
						}
					}
				}
			}
		}
	}

}

void Scene::drawLineWithZBuffer(int x0, int y0, double z0, int x1, int y1, double z1, const Color* color0, const Color* color1) {
    
	// this algorith only sure about the x1 is bigger than x0
	// the cases between y0 and y1 are handled with conditional statements
	// these statements provides us to behave all cases as:
	// slope between 0 and 1 and x0 < x1, meaning in the angle of 0 to -45
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	bool isSlopeHigh = dy > dx;
    // Swap x and y if the s;ope is high
    if (isSlopeHigh) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // Swap start and end points if necessary to ensure left-to-right drawing
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
        std::swap(z0, z1);
		std::swap(color0, color1);
    }

	// Implement as if it is left to right and below the slope of 1
    dx = x1 - x0;
    dy = abs(y1 - y0);
    
	int error = dx / 2;

    int yStep = (y0 < y1) ? 1 : -1;
    int y = y0;

    // Interpolate color and depth
    for (int x = x0; x <= x1; x++) {
        // Calculate the interpolated parameter t
        double t = (x1 == x0) ? 0.0 : static_cast<double>(x - x0) / dx;
        // Ensure color interpolation is consistent with the original start and end points
        Color interpolatedColor = (*color0 * (1.0 - t) + *color1 * t);
        double interpolatedDepth = z0 * (1.0 - t) + z1 * t;
        // Determine the correct coordinates based on whether the slope is high
        int plotX = isSlopeHigh ? y : x;
        int plotY = isSlopeHigh ? x : y;
        // Plot the pixel if it passes the Z-buffer test
        if (plotX >= 0 && plotX < zBuffer.size() && plotY >= 0 && plotY < zBuffer[0].size() && interpolatedDepth < zBuffer[plotX][plotY]) {
            zBuffer[plotX][plotY] = interpolatedDepth;
            image[plotX][plotY] = interpolatedColor;
        }

        // Update error and y coordinate
        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }
}



bool Scene::is_inside(const Vec4* vertex, FrustumPlane plane) {
    switch (plane) {
        case NEAR_PLANE: return vertex->z > -vertex->t;  // Near
        case FAR_PLANE: return vertex->z < vertex->t;   // Far
        case LEFT_PLANE: return vertex->x > -vertex->t;  // Left
        case RIGHT_PLANE: return vertex->x < vertex->t;   // Right
        case BOTTOM_PLANE: return vertex->y > -vertex->t;  // Bottom
        case TOP_PLANE: return vertex->y < vertex->t;   // Top
        default: return true;
    }
}

Vec4* Scene::intersect(const Vec4* v1, const Vec4* v2, FrustumPlane plane) {
    double t;

    switch (plane) {
        case NEAR_PLANE: t = (-v1->t - v1->z) / ((v2->z - v1->z) + (v2->t - v1->t)); break; // Near
        case FAR_PLANE: t = (v1->t - v1->z) / ((v2->z - v1->z) - (v2->t - v1->t)); break;  // Far
        case LEFT_PLANE: t = (-v1->t - v1->x) / ((v2->x - v1->x) + (v2->t - v1->t)); break; // Left
        case RIGHT_PLANE: t = (v1->t - v1->x) / ((v2->x - v1->x) - (v2->t - v1->t)); break;  // Right
        case BOTTOM_PLANE: t = (-v1->t - v1->y) / ((v2->y - v1->y) + (v2->t - v1->t)); break; // Bottom
        case TOP_PLANE: t = (v1->t - v1->y) / ((v2->y - v1->y) - (v2->t - v1->t)); break;  // Top
        default: t = 0.0; break;
    }

    return new Vec4(
        v1->x + t * (v2->x - v1->x),
        v1->y + t * (v2->y - v1->y),
        v1->z + t * (v2->z - v1->z),
        v1->t + t * (v2->t - v1->t),
        v1->colorId
    );
}


std::vector<Vec4*> Scene::clip_against_plane(const std::vector<Vec4*>& vertices, FrustumPlane plane) {
    std::vector<Vec4*> clipped;

    for (size_t i = 0; i < vertices.size(); ++i) {
        Vec4* current = vertices[i];
        Vec4* next = vertices[(i + 1) % vertices.size()];

        // Check if current and next vertices are inside the plane
        bool current_inside = is_inside(current, plane);
        bool next_inside = is_inside(next, plane);
		// cout << " current: " << *current << " inside: " << current_inside << " next: " << *next << " inside: " << next_inside << endl;

        if (current_inside && next_inside) {
            clipped.push_back(next); // Both inside: keep next
        } else if (current_inside && !next_inside) {
            clipped.push_back(intersect(current, next, plane)); // Exiting: add intersection
        } else if (!current_inside && next_inside) {
            clipped.push_back(intersect(current, next, plane)); // Entering: add intersection
            clipped.push_back(next);
        }
    }

    return clipped;
}

std::vector<Vec4*> Scene::clip_triangle(Mesh& mesh, Triangle& triangle) {
    Vec4& v0 = *mesh.projected_vertices[triangle.vertexIds[0]];
    Vec4& v1 = *mesh.projected_vertices[triangle.vertexIds[1]];
    Vec4& v2 = *mesh.projected_vertices[triangle.vertexIds[2]];

	// // test vertices
	// Vec4& v0 = *new Vec4(-3, -3, 0, 5);
	// Vec4& v1 = *new Vec4(-7, -4, 0, 5);
	// Vec4& v2 = *new Vec4(-4, -7, 0, 5);

    // Start with the triangle's vertices
    std::vector<Vec4*> vertices = { &v0, &v1, &v2 };

    // Clip against all six frustum planes
    for (int plane = NEAR_PLANE; plane <= TOP_PLANE; ++plane) {
        vertices = clip_against_plane(vertices, static_cast<FrustumPlane>(plane));
		// cout << "Plane: " << plane << endl;
		// cout << "Vertices: " << endl;
		// for (int i = 0; i < vertices.size(); i++) {
		// 	cout << "Vertex " << i << ": " << *vertices[i] << endl;
		// }
		// cout << "--------------------------" << endl;
        if (vertices.size() < 3) {
            // If less than 3 vertices remain, the triangle is fully clipped
            return {};
        }

    }

	// for (int i = 0; i < vertices.size(); i++) {
	// 	cout << "New Vertex " << i << ": " << *vertices[i] << endl;
	// }

    return vertices;
}

void Scene::clip_wireframe_mesh(Mesh& mesh) {

	std::vector<Vec4*> clipped_vertices;
	std::vector<Triangle* > clipped_triangles;

	for (int i = 0; i < mesh.world_triangles.size(); i++) {
		Triangle& triangle = *mesh.world_triangles[i];

		std::vector<Vec4*> new_triangle_vertices = clip_triangle(mesh, triangle);
		std::vector<int> new_triangle_verices_map_indices;
		int new_vertices_size = new_triangle_vertices.size();
		if (new_vertices_size >= 3) {
			for (int j = 0; j < new_vertices_size; j++) {
				new_triangle_verices_map_indices.push_back(clipped_vertices.size());
				clipped_vertices.push_back(new_triangle_vertices[j]);
			}

			int center_v_id = new_triangle_verices_map_indices[0];
			for (int j = 1; j+1 < new_vertices_size; j++) {
				int v2_id = new_triangle_verices_map_indices[j];
				int v3_id = new_triangle_verices_map_indices[j+1];

				Triangle* new_triangle = new Triangle(center_v_id, v2_id, v3_id);
				clipped_triangles.push_back(new_triangle);
			}
		}
	}

	cout << "clipeed vertices" << endl;
	for (int i = 0; i < clipped_vertices.size(); i++) {
		cout << "Vertex " << i << ": " << *clipped_vertices[i] << endl;
	}
	cout << "clipeed triangles" << endl;
	for (int i = 0; i < clipped_triangles.size(); i++) {
		cout << "Triangle " << i << ": " << clipped_triangles[i] << endl;
	}

	mesh.clipped_triangles = clipped_triangles;
	mesh.clipped_vertices = clipped_vertices;

	

}



void Scene::render() {
	cout << "Rendering..." << endl;

	cout << "Transforming vertices to world space..." << endl;
	for(Mesh *mesh : meshes){
		transform_vertices_to_world(*mesh);
	}

	for (int i = 0; i < cameras.size(); i++)
	{
		Camera* camera = cameras[i];
		cout << "Processing camera " << i << endl;

		cout << "Initializing image..." << endl;
		initializeImage(camera);
		for (Mesh *mesh : meshes)
		{
			cout << "	Processing mesh " << mesh->meshId << endl;

			cout << "	Clearing mesh..." << endl;
			mesh->clear();

			cout << "	Transforming vertices to camera space..." << endl;
			transform_vertices_to_camera(*camera, *mesh);

			cout << "	Projecting camera vertices..." << endl;
			project_camera_vertices(*camera, *mesh);

			cout << "--------------------------" << endl;
			for(int j=0; j<mesh->projected_vertices.size(); j++){
				Vec4* v = mesh->projected_vertices[j];
				cout << "	Projected vertex " << j << ": " << *v << endl;
			}
			cout << "--------------------------" << endl;

            if (mesh->type == WIREFRAME_MESH) {
                cout << "    Clipping wireframe mesh..." << endl;
                clip_wireframe_mesh(*mesh); // Adjusted position
            }
			else
			{
				cout << "	Not Clipping..." << endl;
				mesh->clipped_triangles = mesh->world_triangles;
				mesh->clipped_vertices = mesh->projected_vertices;
			}

			cout << "	Perspective dividing..." << endl;
			perspective_divide(*mesh);

			cout << "	Viewport transforming..." << endl;
			viewport_transform(*camera, *mesh);

			cout << "	Backface culling..." << endl;
			if (cullingEnabled) backface_culling(*camera, *mesh);

			cout << "	Rasterizing..." << endl;
			rasterize(*camera, *mesh);
		}

		myWriteImageToPPMFile(cameras[i]);
		myConvertPPMToPNG(cameras[i]->outputFilename, 1);
	}
}

