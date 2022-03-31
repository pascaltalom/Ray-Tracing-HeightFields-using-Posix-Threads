#ifndef RAYPLANEINTERSECTOR_H
#define RAYPLANEINTERSECTOR_H

#include "Ray.h"
#include "Plane.h"

class RayPlaneIntersector
{
    public:
      static vec3 computeIntersection(Ray& ray, Plane& plane);
};

#endif // RAYPLANEINTERSECTOR_H
