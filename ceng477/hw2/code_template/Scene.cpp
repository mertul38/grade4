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



// MY ADDINGS
void transform_vertices_to_camera(Scene& scene, Camera& camera)
{
    // Ensure camera_vertices is sized correctly
    camera.camera_vertices.resize(scene.world_vertices.size(), nullptr);

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
    for (int i = 0; i < scene.world_vertices.size(); i++)
    {
        Vec3 *worldVertex = scene.world_vertices[i];
        Vec4 extendedVertex = Vec4(worldVertex->x, worldVertex->y, worldVertex->z, 1);
        Vec4 transformedVertex = multiplyMatrixWithVec4(viewMatrix, extendedVertex);

        // Store the transformed vertex in camera_vertices
        camera.camera_vertices[i] = new Vec3(transformedVertex.x, transformedVertex.y, transformedVertex.z);
    }

}

void project_camera_vertices(Camera& camera)
{
    camera.projected_vertices.resize(camera.camera_vertices.size(), nullptr);

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

    for (int i = 0; i < camera.projected_vertices.size(); i++)
    {
        Vec3 *cameraVertex = camera.camera_vertices[i];
        Vec4 extendedVertex = Vec4(cameraVertex->x, cameraVertex->y, cameraVertex->z, 1);
        Vec4 projectedVertex = multiplyMatrixWithVec4(projectionMatrix, extendedVertex);

        camera.projected_vertices[i] = new Vec4(projectedVertex);
        
    }
}


int computeOutcode(Vec4& v){
    int outcode = 0;

    if(v.x < -v.t) outcode |= 1 << 0;
    if (v.x > v.t) outcode |= 1 << 1;
    if (v.y < -v.t) outcode |= 1 << 2;
    if (v.y > v.t) outcode |= 1 << 3;
    if (v.z < -v.t) outcode |= 1 << 4;
    if (v.z > v.t) outcode |= 1 << 5;

    return outcode;
}
void clip(Camera& camera, Mesh& mesh) {
    
    for (Triangle& triangle : mesh.triangles) {
        int v1id = triangle.vertexIds[0];
        int v2id = triangle.vertexIds[1];
        int v3id = triangle.vertexIds[2];

        Vec4& v1 = *camera.projected_vertices[v1id - 1];
        Vec4& v2 = *camera.projected_vertices[v2id - 1];
        Vec4& v3 = *camera.projected_vertices[v3id - 1];

        // Compute outcodes for each vertex
        int outcode1 = computeOutcode(v1);
        int outcode2 = computeOutcode(v2);
        int outcode3 = computeOutcode(v3);

        // cout << "Vertex 1: " << v1 << " Outcode: " << outcode1 << endl;
        // cout << "Vertex 2: " << v2 << " Outcode: " << outcode2 << endl;
        // cout << "Vertex 3: " << v3 << " Outcode: " << outcode3 << endl;

        // Trivial Accept: All vertices are inside
        if ((outcode1 | outcode2 | outcode3) == 0) {
            mesh.clipped_triangles.push_back(triangle);
            continue;
        }

        // Trivial Reject: All vertices share an outside region
        if ((outcode1 & outcode2 & outcode3) != 0) {
            // Entire triangle is outside; skip it
            continue;
        }

        // Non-Trivial Case: Clip the triangle
        // not implemented yed
    }

}

void perspective_divide(Camera& camera) {
    camera.perspected_vertices.resize(camera.projected_vertices.size(), nullptr);
    for (int i = 0; i < camera.projected_vertices.size(); i++) {
        Vec4* v = camera.projected_vertices[i];
        v->x /= v->t;
        v->y /= v->t;
        v->z /= v->t;

        camera.perspected_vertices[i] = new Vec3(v->x, v->y, v->z);
    }
}

void viewportTransform(Camera& camera) {
    // Ensure the viewport-transformed vertices vector is resized
    
    camera.viewport_vertices.resize(camera.perspected_vertices.size(), nullptr);

    // Retrieve screen dimensions from the camera
    int width = camera.horRes;
    int height = camera.verRes;

    // Apply the transformation to each vertex
    for (Vec3* v : camera.perspected_vertices) {
        // Perform the viewport transformation
        double x_screen = (v->x + 1.0) * 0.5 * width;
        double y_screen = (1.0 - v->y) * 0.5 * height;
        double z_screen = (v->z + 1.0) * 0.5; // Normalized depth remains in [0, 1]

        // Store the transformed vertex in the viewportVertices vector
        camera.viewport_vertices.push_back(new Vec3(x_screen, y_screen, z_screen));
    }
}


// MAIN PIPELINE
void Scene::forwardRenderingPipeline(Camera *camera)
{
    // Step 1: Transform vertices to camera space
    transform_vertices_to_camera(*this, *camera);
	cout << "world vertices size: " << world_vertices.size() << " - " << "camera vertices size: " << camera->camera_vertices.size() << endl;
    cout << "-- Vertices transformed to camera space." << endl;
    project_camera_vertices(*camera);
	cout << "camera vertices size: " << camera->camera_vertices.size() << " - " << "projected vertices size: " << camera->projected_vertices.size() << endl;
    cout << "-- Vertices projected to clip space." << endl;
    // Step 3: Process each mesh
    for (Mesh *mesh : meshes)
    {
        clip(*camera, *mesh);
        cout << "-- Triangles clipped." << endl;
        perspective_divide(*camera);
        cout << "-- Triangles perspective divided." << endl;
        viewportTransform(*camera);
        cout << "-- Triangles viewport transformed." << endl;
		break;
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

