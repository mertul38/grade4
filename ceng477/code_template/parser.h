#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include "ppm.h"

using namespace std;

const float VERTEX_EPSILON = 1e-6;

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

        // Vector element-wise multiplication operator
        Vec3f operator*(const Vec3f& other) const {
            return { x * other.x, y * other.y, z * other.z };
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



        void calculateAdditiveInfo() {
            calculateVectorValues();
        }

        void calculateVectorValues(){
            side = up.cross(gaze.negate()).normalize();
            const Vec3f image_center = position + gaze * near_distance;
            image_corner = image_center + side * near_plane.x + up * near_plane.w;
            horizontal_ratio = (near_plane.y - near_plane.x) / image_width;
            vertical_ratio = (near_plane.z - near_plane.w) / image_height;
        }
        void calculateRays(){
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
        Vec3f normal;

        void calculateAdditiveInfo(const std::vector<Vec3f> &vertex_data)
        {
            calculateE1E2(vertex_data);
            calculateNormal();
        }

        void calculateE1E2(const std::vector<Vec3f> &vertex_data){
            E1 = vertex_data[v1_id] - vertex_data[v0_id];
            E2 = vertex_data[v2_id] - vertex_data[v0_id];
        }
     
        void calculateNormal() {
            normal = E1.cross(E2).normalize();
        }
        float rayIntersection (Ray& ray, std::vector<Vec3f> &vertex_data) const{
            Vec3f& v0 = vertex_data[v0_id];
            Vec3f& v1 = vertex_data[v1_id];
            Vec3f& v2 = vertex_data[v2_id];

            Vec3f T = ray.origin - v0;
            Vec3f P = ray.direction.cross(E2);
            Vec3f Q = T.cross(E1);

            float det = P.dot(E1);

            // backface culling
            if (det < 0) {
                return -1.0f;
            }
            // check if ray and triangle are parallel
            if (det < VERTEX_EPSILON) {
                return -1.0f;
            }

            // calculate u
            float u = P.dot(T) / det;
            if (u < 0 || u > 1) {
                return -1.0f;
            }

            // calculate v
            float v = Q.dot(ray.direction) / det;
            if (v < 0 || u + v > 1) {
                return -1.0f;
            }

            // calculate t
            float t = Q.dot(E2) / det;
            if (t < 0) {
                return -1.0f;
            }

            return t;
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
        int center_vertex_id;  // Index of the sphere's center in the vertex_data array
        float radius;

        // Intersection function
        float rayIntersection(const Ray& ray, const std::vector<Vec3f>& vertex_data) const {
            // Retrieve sphere center position from vertex data
            Vec3f center = vertex_data[center_vertex_id];

            // Calculate the vector from the ray's origin to the sphere's center
            Vec3f oc = ray.origin - center;

            // Coefficients for the quadratic equation at^2 + bt + c = 0
            float a = ray.direction.dot(ray.direction);
            float b = 2.0f * oc.dot(ray.direction);
            float c = oc.dot(oc) - radius * radius;

            // Calculate the discriminant
            float discriminant = b * b - 4 * a * c;

            // Check if there is an intersection
            if (discriminant < 0) {
                return -1.0f;  // No intersection
            } else {
                // Calculate the two possible distances to the intersection points
                float sqrt_discriminant = std::sqrt(discriminant);
                float t1 = (-b - sqrt_discriminant) / (2 * a);
                float t2 = (-b + sqrt_discriminant) / (2 * a);

                // Return the closest positive intersection distance, or -1 if none
                if (t1 > 0) return t1;
                if (t2 > 0) return t2;
                return -1.0f;
            }
        }
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
        void calculateAdditiveInfo(){
            // iterate over meshes and triangles and calc face e1 and e2
            cout << "Calculating Camera Parameters" << endl;
            for (int i = 0; i < cameras.size(); i++) {
                Camera& camera = cameras[i];
                camera.calculateAdditiveInfo();
            }
            cout << "Calculating Face Parameters" << endl;
            for (int i = 0; i < meshes.size(); i++) {
                for (int j = 0; j < meshes[i].faces.size(); j++) {
                    meshes[i].faces[j].calculateAdditiveInfo(vertex_data);
                }
            }
            for (int i = 0; i < triangles.size(); i++) {
                triangles[i].indices.calculateAdditiveInfo(vertex_data);
            }
        };

        void calculateSceneImages(){
            for (int i = 0; i < cameras.size(); i++) {
                Camera& camera = cameras[i];
                camera.calculateRays();

            }
        };

        std::vector<unsigned char> imageToPPMdata(Camera& camera, vector<vector<Vec3i>>& image) const{
            std::vector<unsigned char> ppm_data(camera.image_width * camera.image_height * 3);

            for (int j = 0; j < camera.image_height; ++j) {
                for (int i = 0; i < camera.image_width; ++i) {
                    int idx = (j * camera.image_width + i) * 3;
                    ppm_data[idx] = static_cast<unsigned char>(image[j][i].x);  // Red
                    ppm_data[idx + 1] = static_cast<unsigned char>(image[j][i].y);  // Green
                    ppm_data[idx + 2] = static_cast<unsigned char>(image[j][i].z);  // Blue
                }
            }

            return ppm_data;
        }

        void render() {
            cout << "Rendering Scene" << endl;
            int camera_i = 0;
            for (Camera& camera : cameras) {
                cout << "Rendering Camera " << camera_i++ << endl;
                camera.calculateRays();

                vector<vector<Vec3i>> image(camera.image_height, vector<Vec3i>(camera.image_width, background_color));

                for (int j = 0; j < camera.image_height; ++j) {
                    for (int i = 0; i < camera.image_width; ++i) {
                        Ray& ray = camera.image_rays[j][i];
                        float closest_t = -1;
                        Vec3i pixel_color = background_color;
                        Material* hit_material = nullptr;
                        Vec3f intersection_point, normal;

                        // Check intersection with spheres
                        for (const Sphere& sphere : spheres) {
                            float t = sphere.rayIntersection(ray, vertex_data);
                            if (t > 0 && (closest_t < 0 || t < closest_t)) {
                                closest_t = t;
                                hit_material = &materials[sphere.material_id];
                                intersection_point = ray.origin + ray.direction * t;
                                normal = (intersection_point - vertex_data[sphere.center_vertex_id]).normalize();
                            }
                        }

                        // Check intersection with mesh faces
                        for (const Mesh& mesh : meshes) {
                            for (const Face& face : mesh.faces) {
                                float t = face.rayIntersection(ray, vertex_data);
                                if (t > 0 && (closest_t < 0 || t < closest_t)) {
                                    closest_t = t;
                                    hit_material = &materials[mesh.material_id];
                                    intersection_point = ray.origin + ray.direction * t;
                                    normal = face.normal;  // Use precomputed normal
                                }
                            }
                        }

                        // Check intersection with standalone triangles
                        for (const Triangle& triangle : triangles) {
                            float t = triangle.indices.rayIntersection(ray, vertex_data);
                            if (t > 0 && (closest_t < 0 || t < closest_t)) {
                                closest_t = t;
                                hit_material = &materials[triangle.material_id];
                                intersection_point = ray.origin + ray.direction * t;
                                normal = triangle.indices.normal;  // Use precomputed normal
                            }
                        }

                        // If we hit an object, calculate color
                        if (hit_material) {
                            pixel_color = calculateColor(intersection_point, normal, *hit_material, camera.position);
                        }

                        image[j][i] = pixel_color;
                    }
                }

                std::vector<unsigned char> ppm_data = imageToPPMdata(camera, image);
                write_ppm(camera.image_name.c_str(), ppm_data.data(), camera.image_width, camera.image_height);
            }
        }

        Vec3i calculateColor(const Vec3f& intersection_point, const Vec3f& normal, const Material& material, const Vec3f& camera_position) {
            Vec3f ambient_color = ambient_light * material.ambient;

            Vec3f diffuse_color = {0, 0, 0};
            Vec3f specular_color = {0, 0, 0};

            for (const PointLight& light : point_lights) {
                // Direction from intersection point to light
                Vec3f L = (light.position - intersection_point).normalize();

                // Diffuse component
                float diffuse_factor = std::max(0.0f, normal.dot(L));
                diffuse_color = diffuse_color + light.intensity * material.diffuse * diffuse_factor;

                // Specular component
                Vec3f R = (normal * 2 * normal.dot(L) - L).normalize();  // Reflection vector
                Vec3f V = (camera_position - intersection_point).normalize();  // View vector
                float specular_factor = std::pow(std::max(0.0f, R.dot(V)), material.phong_exponent);
                specular_color = specular_color + light.intensity * material.specular * specular_factor;
            }

            // Combine all components
            Vec3f final_color = ambient_color + diffuse_color + specular_color;

            // Clamp colors to the range [0, 255]
            final_color.x = std::min(255.0f, std::max(0.0f, final_color.x));
            final_color.y = std::min(255.0f, std::max(0.0f, final_color.y));
            final_color.z = std::min(255.0f, std::max(0.0f, final_color.z));

            // Return as integer color
            return Vec3i{static_cast<int>(final_color.x), static_cast<int>(final_color.y), static_cast<int>(final_color.z)};
        }


    };
}

#endif