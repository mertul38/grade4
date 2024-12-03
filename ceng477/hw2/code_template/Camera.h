#ifndef __CAMERA_H__
#define __CAMERA_H__
#define ORTOGRAPHIC_PROJECTION 0
#define PERSPECTIVE_PROJECTION 1
#include "Vec3.h"
#include "Vec4.h"
#include <vector>

class Camera
{

public:
    int cameraId, projectionType, horRes, verRes; // projectionType=0 for orthographic, projectionType=1 for perspective
    Vec3 position, gaze, u, v, w;
    double left, right, bottom, top, near, far;
    std::string outputFilename;

    // MY ADDINGS
    std::vector<Vec3 *> camera_vertices;
    std::vector<Vec4 *> projected_vertices;
    std::vector<Vec3 *> perspected_vertices;
    std::vector<Vec3 *> viewport_vertices;

    Camera();

    Camera(int cameraId,
           int projectionType,
           Vec3 position, Vec3 gaze,
           Vec3 u, Vec3 v, Vec3 w,
           double left, double right, double bottom, double top,
           double near, double far,
           int horRes, int verRes,
           std::string outputFilename);

    Camera(const Camera &other);

    friend std::ostream &operator<<(std::ostream &os, const Camera &c);
};

#endif