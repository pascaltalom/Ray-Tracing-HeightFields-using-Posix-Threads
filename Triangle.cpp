#include "Triangle.h"

Triangle::Triangle(const vec3& A,const  vec3& B, const vec3& C)
{
    vec3 ba = A-B;
    vec3 bc = C-B;
    vec3 triangleNormal = normalize(cross(bc,ba));
    trianglePlane = new Plane(B,triangleNormal);

    vec3 aNormal = cross(triangleNormal,bc);
    aPlane = new Plane(C,aNormal);

    vec3 ca = A-C;
    vec3 bNormal = cross(triangleNormal,ca);
    bPlane = new Plane(A,bNormal);

    vec3 cNormal = cross(ba,triangleNormal);
    cPlane = new Plane(A,cNormal);
    //ctor
}

Triangle::~Triangle()
{
    delete cPlane;
    cPlane = 0;

    delete bPlane;
    bPlane = 0;

    delete aPlane;
    aPlane = 0;

    delete trianglePlane;
    trianglePlane = 0;
}

IntersectionData Triangle::computeIntersection(Ray& ray)
{
    IntersectionData intersectionData = trianglePlane->computeIntersection(ray);
    if(!intersectionData.doesIntersect)
        return intersectionData;

    vec3 intersection = intersectionData.position;
    if(aPlane->isInFront(intersection)&&bPlane->isInFront(intersection)&& cPlane->isInFront(intersection))
    {
        intersectionData.doesIntersect = true;
        intersectionData.brightness = trianglePlane->getNormalVector().y;
        if(intersectionData.brightness < 0)
            intersectionData.brightness = 0;
    }
    else{
        intersectionData.doesIntersect = false;
    }
    return intersectionData;
}
