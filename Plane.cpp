#include "Plane.h"

Plane::Plane(const vec3& position, const vec3& normalVector):position(position),normalVector(normalVector)
{
    dotProductPositionNormal = dot(position,normalVector);
}

Plane::~Plane()
{
}

bool Plane::isInFront(vec3& p)
{
    bool inFront = dot(p-position,normalVector) >= 0;
    return inFront;
}

IntersectionData Plane::computeIntersection(Ray& ray)
{
    IntersectionData result;

    float dotDirectionNormal = dot(ray.getDirection(),normalVector);

    if(dotDirectionNormal == 0)
    {
        result.doesIntersect = false;
        return result;
    }

    float lambda = (dotProductPositionNormal - dot(ray.getPosition(), normalVector))/dotDirectionNormal;

    if(lambda < 0){

        result.doesIntersect = false;
        return result;
    }

    result.doesIntersect = true;
    result.position = ray.computePositionOnRay(lambda);
    result.lambda = lambda;

    return result;
}
