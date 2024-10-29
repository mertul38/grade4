#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include "ppm.h"
#include <tuple>

using namespace std;

const float VERTEX_EPSILON = 1e-6;
const float FLOAT_COMP_EPSILON = 1e-3;


namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    inline bool compareFloats(float a, float b) {
        return abs(a - b) < FLOAT_COMP_EPSILON;
    }

    struct Vec3f 
    {
        float x, y, z;

        // my addings
        // Addition operator
        Vec3f operator+(const Vec3f& other) const {
            return { x + other.x, y + other.y, z + other.z };
        }

        Vec3f operator+(const float scalar) const {
            return { x + scalar, y + scalar, z + scalar };
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

        Vec3f operator/(float scalar) const {
            return { x / scalar, y / scalar, z / scalar };
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
    // my adding
    struct Ray
    {
        Vec3f origin;
        Vec3f direction;
        
        // my addings
        string toString() const {
            return origin.toString() + " " + direction.toString();
        }
    };

    inline const Vec3f* getVertexDataById(const std::vector<Vec3f> &vertex_data ,int id) {
        return &vertex_data[id-1];
    }

    struct Vec3i
    {
        int x, y, z;

        std::string toString() const {
            return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);    
        }
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
            vertical_ratio = (near_plane.w - near_plane.z) / image_height;
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
            const Vec3f s = image_corner + side * su + up * -sv;
            const Vec3f direction = (s - position).normalize();
            return {position, direction};

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

    inline const Material* getMaterialById(const std::vector<Material> &material_data ,int id) {
        return &material_data[id-1];
    }

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
            E1 = *getVertexDataById(vertex_data, v1_id) - *getVertexDataById(vertex_data, v0_id);
            E2 = *getVertexDataById(vertex_data, v2_id) - *getVertexDataById(vertex_data, v0_id);    
        }
     
        void calculateNormal() {
            normal = E1.cross(E2).normalize();
        }
        
        float rayIntersection (Ray& ray, const std::vector<Vec3f> &vertex_data) const{
            const Vec3f& v0 = *getVertexDataById(vertex_data, v0_id);
            const Vec3f& v1 = *getVertexDataById(vertex_data, v1_id);
            const Vec3f& v2 = *getVertexDataById(vertex_data, v2_id);

            Vec3f P = ray.direction.cross(E2);
            float det = P.dot(E1);

            // check if ray and triangle are parallel
            if (det < 0) {
                return -1.0f;
            }

            float invDet = 1.0f / det;

            Vec3f T = ray.origin - v0;
            float u = T.dot(P) * invDet;
            if (u < 0 || u > 1) {
                return -1.0f;
            }

            Vec3f Q = T.cross(E1);
            float v = ray.direction.dot(Q) * invDet;
            if (v < 0 || u + v > 1) {
                return -1.0f;
            }

            // calculate t
            float t = E2.dot(Q) * invDet;

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
            Vec3f center = *getVertexDataById(vertex_data, center_vertex_id);

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
        vector<Vec3i> colors = {
            {0, 0, 0}, // Black
            {255, 0, 0}, // Red
            {0, 255, 0}, // Green
            {0, 0, 255}, // Blue
            {255, 255, 0}, // Yellow
            {0, 255, 255}, // Cyan
            {255, 0, 255}, // Magenta
            {255, 255, 255} // White
        };
        int log_j = 50;
        int log_i = 240;
        bool flag;
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
            for (int i = 0; i < cameras.size(); i++) {
                Camera& camera = cameras[i];
                camera.calculateAdditiveInfo();
            }
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

        tuple<float, Vec3f, Vec3f, const Material*, int> findIntersection(Ray& ray) const {
            float closest_t = -1;
            const Material* hit_material = nullptr;
            Vec3f intersection_point, normal;
            string log;
            int mesh_id = -1;
            // Check intersection with spheres
            int sphere_i = 0;
            for (const Sphere& sphere : spheres) {
                sphere_i++;
                float t = sphere.rayIntersection(ray, vertex_data);
                if (t > 0 && (closest_t < 0 || t < closest_t)) {
                    closest_t = t;
                    hit_material = getMaterialById(materials, sphere.material_id);
                    intersection_point = ray.origin + ray.direction * t;
                    normal = (intersection_point - *getVertexDataById(vertex_data, sphere.center_vertex_id)).normalize();
                    log = "sphere_id: " + to_string(sphere_i);
                }
            }

            // Check intersection with mesh faces
            int mesh_i = 0;
            for (const Mesh& mesh : meshes) {
                mesh_i++;
                int face_i = 0;
                for (const Face& face : mesh.faces) {
                    face_i++;
                    float t = face.rayIntersection(ray, vertex_data);
                    if (t > 0 && (closest_t < 0 || t < closest_t)) {
                        closest_t = t;
                        hit_material = getMaterialById(materials, mesh.material_id);
                        intersection_point = ray.origin + ray.direction * t;
                        normal = face.normal;  // Use precomputed normal
                        mesh_id = mesh_i;
                        log = "mesh_id: " + to_string(mesh_i) + ", face_id: " + to_string(face_i);
                    }
                }
            }

            // Check intersection with standalone triangles
            int triangle_i = 0;
            for (const Triangle& triangle : triangles) {
                triangle_i++;
                float t = triangle.indices.rayIntersection(ray, vertex_data);
                if (t > 0 && (closest_t < 0 || t < closest_t)) {
                    closest_t = t;
                    hit_material = getMaterialById(materials, triangle.material_id);
                    intersection_point = ray.origin + ray.direction * t;
                    normal = triangle.indices.normal;  // Use precomputed normal
                    log = "triangle_id: " + to_string(triangle_i);
                }
            }

            if (flag) cout << log << endl;
            return std::make_tuple(closest_t, intersection_point, normal, hit_material, mesh_id);

        }

        Vec3i calculateColor(const Ray& incoming_ray, const Vec3f& intersection_point, const Vec3f& normal, const Material& material, int depth) {
            // Start with ambient color
            Vec3f color = material.ambient * ambient_light;
            
            // Calculate view direction
            Vec3f view_dir = (incoming_ray.origin - intersection_point).normalize();

            // Diffuse and Specular lighting computation]
            if (flag) cout << "Lighting calculation" << endl;
            for (const PointLight& light : point_lights) {
                Vec3f light_dir = (light.position - intersection_point);
                Vec3f light_dir_normalized = light_dir.normalize();
                // Shadow ray check to see if the point is shadowed
                Ray shadow_ray = {intersection_point + normal * shadow_ray_epsilon, light_dir_normalized};
                float closest_t;
                Vec3f shadow_intersection, shadow_normal;
                const Material* shadow_material;
                int mesh_id;

                std::tie(closest_t, shadow_intersection, shadow_normal, shadow_material,mesh_id) = findIntersection(shadow_ray);
                if (closest_t > 0 && closest_t < light_dir.length()) {
                    if (flag) cout << "Point is in shadow" << endl;
                    continue; // Skip this light as the point is in shadow
                }

                // Lighting calculation if not in shadow
                float distance = light_dir.length();
                Vec3f light_intensity = light.intensity / (distance * distance);

                // Diffuse component
                float diff_factor = std::max(normal.dot(light_dir_normalized), 0.0f);
                Vec3f diffuse = material.diffuse * light_intensity * diff_factor;

                // Specular component using Phong model
                Vec3f halfway = (view_dir + light_dir_normalized).normalize();
                float spec_factor = std::pow(std::max(normal.dot(halfway), 0.0f), material.phong_exponent);
                Vec3f specular = material.specular * light_intensity * spec_factor;

                color = color + diffuse + specular;
            }

            // **Mirror Reflection Calculation** (recursive step)
            if (material.is_mirror && depth < max_recursion_depth) {
                if (flag) cout << "Mirror Reflection" << endl;
                // Calculate reflection direction R = I - 2 * (I . N) * N
                Vec3f reflection_dir = incoming_ray.direction - normal * 2.0f * incoming_ray.direction.dot(normal);
                Ray reflection_ray = {intersection_point + normal * shadow_ray_epsilon, reflection_dir};

                // Recursively trace the reflection ray
                float reflection_t;
                Vec3f reflection_intersection, reflection_normal;
                const Material* reflection_material;
                int mesh_id;
                std::tie(reflection_t, reflection_intersection, reflection_normal, reflection_material,mesh_id) = findIntersection(reflection_ray);

                // If the reflection ray hits an object, calculate its color
                if (reflection_t > 0 && reflection_material) {
                    Vec3i reflection_color = calculateColor(reflection_ray, reflection_intersection, reflection_normal, *reflection_material, depth + 1);
                    Vec3f reflection_color_f = {
                        static_cast<float>(reflection_color.x),
                        static_cast<float>(reflection_color.y),
                        static_cast<float>(reflection_color.z)
                    };
                    // Combine reflection color based on the mirror reflectance of the material
                    color = color + material.mirror * reflection_color_f;
                }
            }

            // Clamp color values to [0, 255] range
            color.x = std::max(0.0f, std::min(255.0f, color.x));
            color.y = std::max(0.0f, std::min(255.0f, color.y));
            color.z = std::max(0.0f, std::min(255.0f, color.z));

            return {static_cast<int>(color.x), static_cast<int>(color.y), static_cast<int>(color.z)};
        }

        void render() {
            int camera_i = 0;
            for (Camera& camera : cameras) {
                cout << "---------------------------" << endl << "Rendering Camera " << camera_i++ << endl;
                camera.calculateRays();

                vector<vector<Vec3i>> image(camera.image_height, vector<Vec3i>(camera.image_width, background_color));

                for (int j = 0; j < camera.image_height; ++j) {
                    for (int i = 0; i < camera.image_width; ++i) {
                        if (j == log_j && i == log_i) {
                            flag = true;
                        }
                        Ray& camera_ray = camera.image_rays[j][i];
                        
                        // Declare the variables
                        float closest_t;
                        Vec3f intersection_point;
                        Vec3f normal;
                        const Material* hit_material;
                        int mesh_id;
                        // Call the function and assign each return value from the tuple
                        if (flag) cout << "Primary Ray" << endl;
                        std::tie(closest_t, intersection_point, normal, hit_material, mesh_id) = findIntersection(camera_ray);

                        Vec3i pixel_color = background_color;
                        // If we hit an object, calculate color
                        if (closest_t > 0) {
                            if (mesh_id == -2) {
                                pixel_color = colors[mesh_id];
                            }
                            else{
                                pixel_color = calculateColor(camera_ray, intersection_point, normal, *hit_material, 0);
                            }
                            // pixel_color = {255, 0, 0};
                            // cout << pixel_color.toString() << endl;

                        }

                        image[j][i] = pixel_color;
                        flag = false;
                    }
                }

                std::vector<unsigned char> ppm_data = imageToPPMdata(camera, image);
                write_ppm(camera.image_name.c_str(), ppm_data.data(), camera.image_width, camera.image_height);
            }
        }


    };
}

#endif