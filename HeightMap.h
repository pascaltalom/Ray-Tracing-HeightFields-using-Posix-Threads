#ifndef HEIGHMAP_H
#define HEIGHMAP_H

#include <vector>

#include "Triangle.h"
#include "DDAStartIntersection.h"
#include "Plane.h"

class HeightMap
{
    public:
        HeightMap(float topLeftX, float topLeftY, float topLeftZ, float diameterQuad, int numRows, int numColumns, float* heights);
        virtual ~HeightMap();
        IntersectionData computeIntersection(Ray& rayColumn);
        struct HeighMapStartIntersection computeDDAStartIntersection(Ray& ray);

    protected:
        IntersectionData computeIntersectionTriangleQuad(Ray& ray, int row, int column);
    private:
        std::vector<Triangle*> triangles;
        vec3 sphereCenter;
        float squaredRadius;

        std::vector<Plane*> borderPlanes;
        vec3 topLeftPosition;
        vec3 bottomRightPosition;

        float diameterQuad;
        int numRows;
        int numColumns;

        float minHeight, maxHeight;
};

#endif // HEIGHMAP_H
