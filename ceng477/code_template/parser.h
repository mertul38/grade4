#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f 
    {
        float x, y, z;

        // my addings
        // Addition operator
        Vec3f operator+(const Vec3f& other) const {
            return { x + other.x, y + other.y, z + other.z };
        }

        // Subtraction operator
        Vec3f operator-(const Vec3f& other) const {
            return { x - other.x, y - other.y, z - other.z };
        }

        // Scalar multiplication operator
        Vec3f operator*(float scalar) const {
            return { x * scalar, y * scalar, z * scalar };
        }

        // Dot product
        float dot(const Vec3f& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        // Cross product
        Vec3f cross(const Vec3f& other) const {
            return {
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            };
        }

        // Negate the vector
        Vec3f negate() const {
            return { -x, -y, -z };
        }

        // Length (magnitude) of the vector
        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        // Normalize function that returns a normalized Vec3f
        Vec3f normalize() const {
            float len = length();
            return { x / len, y / len, z / len };
        }

        std::string toString() const {
            return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
        }
    };

    struct Ray
    {
        Vec3f origin;
        Vec3f direction;
        
        // my addings
        string toString() const {
            return origin.toString() + " " + direction.toString();
        }
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;

        // my addings
        Vec3f side;
        Vec3f image_corner;
        float horizontal_ratio;
        float vertical_ratio;
        vector<vector<Ray>> image_rays;



        void calculateAdditiveCameraParameters() {
            side = up.cross(gaze.negate()).normalize();
            const Vec3f image_center = position + gaze * near_distance;
            image_corner = image_center + side * near_plane.x + up * near_plane.w;
            horizontal_ratio = (near_plane.y - near_plane.x) / image_width;
            vertical_ratio = (near_plane.z - near_plane.w) / image_height;

        }

        void generateAllRays(){
            image_rays = vector<vector<Ray>>(image_height, vector<Ray>(image_width));
            for (int j = 0; j < image_height; ++j) {      // Vertical axis (rows)
                for (int i = 0; i < image_width; ++i) {   // Horizontal axis (columns)
                    // Generate a ray for pixel (i, j)
                    Ray ray = generateRay(i, j);
                    image_rays[j][i] = ray;
                }
            }


        };

        Ray generateRay(int i, int j){

            // calculate u and v for pixel (i, j)
            const float su = (i + 0.5f) * horizontal_ratio;
            const float sv = (j + 0.5f) * vertical_ratio;
            const Vec3f s = image_corner + side * su + up * sv;
            const Vec3f direction = (s - position).normalize();
            return {position, direction};
            // 

            // // Calculate u and v for pixel (i, j)
            // float u = left + (right - left) * (i + 0.5f) / image_width;
            // float v = bottom + (top - bottom) * (j + 0.5f) / image_height;

            // // Calculate ray direction from camera through pixel (i, j)
            // cout << "u: " << u << " v: " << v << endl;
            // Vec3f ray_direction = (
            //     gaze * near_distance + side * u + up * v
            // ).normalize();

            // return {position, ray_direction};
        };

    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        bool is_mirror;
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;

        
        Vec3f E1;
        Vec3f E2;

        void calculateE1E2(const std::vector<Vec3f> &vertex_data)
        {
            E1 = vertex_data[v1_id] - vertex_data[v0_id];
            E2 = vertex_data[v2_id] - vertex_data[v0_id];
        }
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };

    struct Triangle
    {
        int material_id;
        Face indices;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        //Functions
        void loadFromXml(const std::string &filepath);
        void calculateAdditiveParameters(){
            // iterate over meshes and triangles and calc face e1 and e2
            cout << "Calculating Camera Parameters" << endl;
            for (int i = 0; i < cameras.size(); i++) {
                Camera& camera = cameras[i];
                camera.calculateAdditiveCameraParameters();
                camera.generateAllRays();
            }
            cout << "Calculating Mesh Parameters" << endl;
            for (int i = 0; i < meshes.size(); i++) {
                for (int j = 0; j < meshes[i].faces.size(); j++) {
                    meshes[i].faces[j].calculateE1E2(vertex_data);
                }
            }
            cout << "Calculating Triangle Parameters" << endl;
            for (int i = 0; i < triangles.size(); i++) {
                triangles[i].indices.calculateE1E2(vertex_data);
            }
        };

    };
}

#endif