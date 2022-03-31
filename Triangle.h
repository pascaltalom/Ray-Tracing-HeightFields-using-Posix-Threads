#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Plane.h"

class Triangle
{
    public:
        Triangle(const vec3& A,const  vec3& B,const  vec3& C);
        virtual ~Triangle();
        IntersectionData computeIntersection(Ray& ray);

    protected:

    private:

    Plane* trianglePlane;
    Plane* aPlane;
    Plane* bPlane;
    Plane* cPlane;
};

#endif // TRIANGLE_H
