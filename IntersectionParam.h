#ifndef INTERSECTION_PARAMETER
#define INTERSECTION_PARAMETER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

struct IntersectionData
{
    vec3 position;
    bool doesIntersect;
    float lambda;
    float brightness;
};


#endif // INTERSECTION_PARAMETER
