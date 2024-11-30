import numpy as np
import trimesh

# Function to calculate ray-triangle intersection using trimesh
def ray_triangle_intersection(ray_origin, ray_direction, triangle_vertices):
    # Create a ray and a mesh object for the triangle
    ray_origin = np.array(ray_origin)
    ray_direction = np.array(ray_direction)
    triangle = trimesh.Trimesh(vertices=triangle_vertices, faces=[[0, 1, 2]])

    # Use the mesh-ray intersection method from trimesh
    locations, index_ray, index_tri = triangle.ray.intersects_location(
        ray_origins=[ray_origin], ray_directions=[ray_direction]
    )

    # Check if there is an intersection
    if len(locations) > 0:
        return locations[0]  # Return the intersection point
    else:
        return None  # No intersection

# Function to calculate a ray from camera parameters
def calculate_ray(u, v, w, image_plane_size, image_plane_distance, left, right, bottom, top, pixel_x, pixel_y):
    image_width, image_height = image_plane_size
    # Calculate the image plane coordinates for the given pixel
    pixel_width = (right - left) / image_width
    pixel_height = (top - bottom) / image_height
    su = (pixel_x + 0.5) * pixel_width
    sv = (pixel_y + 0.5) * pixel_height

    image_center = image_plane_distance * -w
    image_topleft = image_center + left * u + top * v

    # Calculate the ray direction
    ray_direction = image_topleft + su * u + -sv * v
    return ray_direction / np.linalg.norm(ray_direction)


# Test example
def main():
    ray_origin = [0, 0, 0]
    ray_direction = [0, 0, 1]
    triangle_vertices = [
        [0, 1, 5],
        [1, -1, 5],
        [-1, -1, 5]
    ]

    intersection_point = ray_triangle_intersection(ray_origin, ray_direction, triangle_vertices)
    if intersection_point is not None:
        print(f"Intersection point: {intersection_point}")
    else:
        print("No intersection")

    # Camera parameters for ray calculation
    camera_position = np.array([0, 0, 0])
    u = np.array([1, 0, 0])
    v = np.array([0, 1, 0])
    w = np.array([0, 0, 1])
    image_plane_size = (1024, 768)
    image_plane_distance = 1.0
    left = -1
    right = 1
    bottom = -1
    top = 1
    pixel_x = 256
    pixel_y = 192

    ray_direction = calculate_ray(u, v, w, image_plane_size, image_plane_distance, left, right, bottom, top, pixel_x, pixel_y)
    print(f"Ray direction for pixel ({pixel_x}, {pixel_y}): {ray_direction}")

if __name__ == "__main__":
    main()