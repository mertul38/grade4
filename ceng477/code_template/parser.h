#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <cmath>
#include <string>
#include <vector>

namespace parser {

    // CRTP Base Template Class for Vec3
    template <typename T, typename Derived>
    class Vec3 {
    public:
        T x, y, z;

        // Constructor
        Vec3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

        // Addition operator
        Derived operator+(const Derived& other) const {
            return Derived(x + other.x, y + other.y, z + other.z);
        }

        // Subtraction operator
        Derived operator-(const Derived& other) const {
            return Derived(x - other.x, y - other.y, z - other.z);
        }

        // Scalar multiplication operator
        Derived operator*(T scalar) const {
            return Derived(x * scalar, y * scalar, z * scalar);
        }

        // Dot product
        T dot(const Derived& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        // Cross product
        Derived cross(const Derived& other) const {
            return Derived(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // Negate the vector
        Derived negate() const {
            return Derived(-x, -y, -z);
        }
    
        void printVec3f(const std::string& prefix, const Derived& v) {
            std::cout << prefix << ": " <<  v.x << " " << v.y << " " << v.z << std::endl;
        }
    };

    // Derived class Vec3f using float type
    class Vec3f : public Vec3<float, Vec3f> {
    public:
        using Vec3<float, Vec3f>::Vec3; // Inherit constructors

        // Additional method for Vec3f: Length and normalization
        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vec3f normalize() const {
            float len = length();
            return Vec3f(x / len, y / len, z / len);
        }
    };

    class Ray {
    public:
        Vec3f origin;
        Vec3f direction;
        
        Ray(const Vec3f& origin, const Vec3f& direction) 
        : origin(origin), direction(direction.normalize()) {
        }
    };
    // Derived class Vec3i using int type
    class Vec3i : public Vec3<int, Vec3i> {
    public:
        using Vec3<int, Vec3i>::Vec3; // Inherit constructors
    };

    // Vec4f class
    class Vec4f {
    public:
        float x, y, z, w;
    };

    // Other classes: Camera, PointLight, Material, Face, Mesh, Triangle, Sphere, Scene
    class Camera {
    public:
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    class PointLight {
    public:
        Vec3f position;
        Vec3f intensity;
    };

    class Material {
    public:
        bool is_mirror;
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    class Face {
    public:
        int v0_id;
        int v1_id;
        int v2_id;
    };

    class Mesh {
    public:
        int material_id;
        std::vector<Face> faces;
    };

    class Triangle {
    public:
        int material_id;
        Face indices;
        Vec3f E1;
        Vec3f E2;
        

    };

    class Sphere {
    public:
        int material_id;
        int center_vertex_id;
        float radius;
    };

    class Scene {
    public:
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

        void loadFromXml(const std::string &filepath);
    };
}

#endif
