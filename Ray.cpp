#include "Ray.h"

Ray::Ray(const vec3& position,const vec3& direction):position(position),direction(direction)
{
    //ctor
}

Ray::~Ray()
{
    //dtor
}

vec3 Ray::computePositionOnRay(float lambda)
{
    return position + lambda*direction;
}
