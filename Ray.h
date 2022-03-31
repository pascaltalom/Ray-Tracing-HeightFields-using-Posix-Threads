#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Ray
{
    public:
        Ray(const vec3& position,const vec3& direction);
        virtual ~Ray();

        vec3& getPosition()
        {
            return position;
        }

        vec3& getDirection()
        {
            return direction;
        }

        vec3 computePositionOnRay(float lambda);
    private:
        vec3 position;
        vec3 direction;
};

#endif // RAY_H
