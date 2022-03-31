#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Ray.h"
#include "IntersectionParam.h"
using namespace glm;

class Plane
{
    public:
        Plane(const vec3& position, const vec3& normalVector);
        virtual ~Plane();


        IntersectionData computeIntersection(Ray& ray);


        bool isInFront(vec3& p);

        vec3 getNormalVector()
        {
            return normalVector;
        }
    protected:

    private:

        vec3 position;
        vec3 normalVector;
        float dotProductPositionNormal;
};
#endif // PLANE_H
