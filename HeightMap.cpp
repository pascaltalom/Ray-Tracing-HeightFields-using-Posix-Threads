#include "HeightMap.h"

#include <float.h>

#include <iostream>

HeightMap::HeightMap(float topLeftX, float topLeftY, float topLeftZ, float diameterQuad, int numRows, int numColumns, float* heights):
diameterQuad(diameterQuad),
numRows(numRows),
numColumns(numColumns)
{


    const float width = (numColumns-1)*diameterQuad;
    const float height = (numRows-1)*diameterQuad;

    topLeftPosition.x= topLeftX;
    topLeftPosition.y = topLeftY;
    topLeftPosition.z = topLeftZ;

    bottomRightPosition.x = topLeftX + width;
    bottomRightPosition.y = topLeftY;
    bottomRightPosition.z = topLeftZ + height;


    borderPlanes.push_back(new Plane(topLeftPosition, vec3(0,0,-1.0f)));
    borderPlanes.push_back(new Plane(vec3(topLeftX,topLeftY, topLeftZ+height),vec3(0,0,1.0f)));
    borderPlanes.push_back(new Plane(topLeftPosition,vec3(-1.0f,0,0)));
    borderPlanes.push_back(new Plane(vec3(topLeftX+width,topLeftY, topLeftZ),vec3(1.0f,0,0)));


    sphereCenter = vec3(topLeftX+numColumns*diameterQuad/2.0f,topLeftY,topLeftZ + numRows*diameterQuad/2.0f);
    squaredRadius = 0;

    //Calculate the maximum and minimum height
    maxHeight = heights[0]+topLeftY;
    minHeight = heights[0]+topLeftY;
    for(int i = 1; i < numRows*numColumns; i++ )
    {
    	if(heights[i]+topLeftY < minHeight)
    	{
    		minHeight = heights[i]+topLeftY;
    	}
    	if(heights[i]+topLeftY > maxHeight)
    	{
    		maxHeight = heights[i]+topLeftY;
    	}
    }


    for(int quadRow = 0; quadRow < numRows-1; quadRow++)
    {
        for(int quadCol = 0; quadCol < numColumns-1; quadCol++)
        {
            float squaredDist = 0;
            vec3 diff;

            float quadTopLeftX = topLeftX + quadCol*diameterQuad;
            float quadTopLeftZ = topLeftZ + quadRow*diameterQuad;
            float quadTopLeftY = topLeftY + heights[quadRow*numColumns+quadCol];

            vec3 quadTopLeft(quadTopLeftX,quadTopLeftY,quadTopLeftZ);
            diff = quadTopLeft-sphereCenter;
            squaredDist = dot(diff,diff);
            if(squaredDist > squaredRadius)
                squaredRadius = squaredDist;

            float quadTopRightX = quadTopLeftX + diameterQuad;
            float quadTopRightZ = quadTopLeftZ;
            float quadTopRightY = topLeftY + heights[quadRow*numColumns+quadCol+1];

            vec3 quadTopRight(quadTopRightX,quadTopRightY,quadTopRightZ);
            diff = quadTopRight-sphereCenter;
            squaredDist = dot(diff,diff);
            if(squaredDist > squaredRadius)
                squaredRadius = squaredDist;

            float quadBottonRightX = quadTopLeftX + diameterQuad;
            float quadBottonRightZ = quadTopLeftZ + diameterQuad;
            float quadBottonRightY = topLeftY + heights[(quadRow+1)*numColumns+quadCol+1];

            vec3 quadBottonRight(quadBottonRightX,quadBottonRightY,quadBottonRightZ);
            diff = quadBottonRight-sphereCenter;
            squaredDist = dot(diff,diff);
            if(squaredDist > squaredRadius)
                squaredRadius = squaredDist;

            float quadBottonLeftX = quadTopLeftX;
            float quadBottonLeftZ = quadTopLeftZ + diameterQuad;
            float quadBottonLeftY = topLeftY + heights[(quadRow+1)*numColumns+quadCol];

            vec3 quadBottonLeft(quadBottonLeftX,quadBottonLeftY,quadBottonLeftZ);
            diff = quadBottonLeft-sphereCenter;
            squaredDist = dot(diff,diff);
            if(squaredDist > squaredRadius)
                squaredRadius = squaredDist;

            triangles.push_back(new Triangle(quadTopLeft,quadBottonLeft,quadBottonRight));
            triangles.push_back(new Triangle(quadBottonRight,quadTopRight,quadTopLeft));
        }
    }


}

HeightMap::~HeightMap()
{
    for(int i = 0; i < borderPlanes.size(); i++)
    {
        delete borderPlanes[i];
    }
    borderPlanes.clear();
    for(int i = 0; i < triangles.size(); i++)
    {
        delete triangles[i];
    }
    triangles.clear();
}

struct HeighMapStartIntersection HeightMap::computeDDAStartIntersection(Ray& ray)
{
    struct HeighMapStartIntersection result;
    result.doesIntersect = false;
    result.row = -1;
    result.column = -1;


    float lambdaMin = FLT_MAX;

    bool intersectsPlane = false;

    for(int i = 0; i < borderPlanes.size(); i++)
    {
         IntersectionData currentIntersection = borderPlanes[i]->computeIntersection(ray);
         if(currentIntersection.doesIntersect)
         {
            if(currentIntersection.lambda < lambdaMin)
            {
                vec3 intersection = ray.computePositionOnRay(currentIntersection.lambda);

                //Has the ray really touched the heighmap?
                const float epsilon = 0.01f;
                if(intersection.x < topLeftPosition.x-epsilon || bottomRightPosition.x+epsilon < intersection.x)
                {
                    continue;
                }

                if(intersection.z < topLeftPosition.z-epsilon  || bottomRightPosition.z+epsilon < intersection.z)
                {
                     continue;
                }

                 lambdaMin = currentIntersection.lambda;
                 intersectsPlane = true;
             }
         }
    }

    if(!intersectsPlane)
    {
        return result;
    }

    vec3 intersection = ray.computePositionOnRay(lambdaMin);

    result.row = (intersection.z-topLeftPosition.z)/diameterQuad;
    result.row = max(0, min(numRows-2,result.row));



    result.column = (intersection.x-topLeftPosition.x)/diameterQuad;
    result.column = max(0, min(numColumns-2,result.column));

    result.intersectionX = intersection.x;
    result.intersectionZ = intersection.z;
    result.intersectionY = intersection.y;

    result.doesIntersect = true;

    return result;
};

IntersectionData HeightMap::computeIntersectionTriangleQuad(Ray& ray, int row, int column)
{
	int startIndexTriangles = (row*(numColumns-1)+column)*2;
	struct IntersectionData nearestIntersection;
	nearestIntersection.doesIntersect = false;
	for(int i = 0; i < 2; i++)
	{
	  IntersectionData currentIntersection = triangles[startIndexTriangles+i]->computeIntersection(ray);
	  if(currentIntersection.doesIntersect)
	  {
		  if(currentIntersection.lambda < nearestIntersection.lambda || nearestIntersection.doesIntersect == false)
		  {
			nearestIntersection = currentIntersection;
		  }
      }
    }
	return nearestIntersection;
}

IntersectionData HeightMap::computeIntersection(Ray& ray)
	{
	    IntersectionData nearestIntersection;
	    nearestIntersection.doesIntersect = false;


	    //Check if the bounding sphere is touched
	    float a = dot(ray.getDirection(),sphereCenter);
	    float b = dot(sphereCenter,sphereCenter);
	    float delta = a*a - (b - squaredRadius);
	    if(delta < 0)
	        return nearestIntersection;


	    //Where was the bounding box touched?
	    HeighMapStartIntersection startIntersection = computeDDAStartIntersection(ray);

	    if(!startIntersection.doesIntersect)
	        return nearestIntersection;

	    if(ray.getDirection().y > 0 && startIntersection.intersectionY > maxHeight)
	    {
	    	return nearestIntersection;
	    }

	    if(ray.getDirection().y < 0 && startIntersection.intersectionY < minHeight)
	    {
	    	return nearestIntersection;
	    }

	    int currentRow = startIntersection.row;
		int currentColumn = startIntersection.column;

		float deltaDistX = sqrtf(1.0f + ray.getDirection().z*ray.getDirection().z/ (ray.getDirection().x*ray.getDirection().x));
		float deltaDistZ = sqrtf(1.0f + ray.getDirection().x*ray.getDirection().x/ (ray.getDirection().z*ray.getDirection().z));

		int stepX;
		int stepZ;

		float sideDistX = 0;
		float sideDistZ = 0;

		if(ray.getDirection().x < 0)
		{
			stepX = -1;
			sideDistX = (startIntersection.intersectionX-currentColumn-topLeftPosition.x)*deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (currentColumn + topLeftPosition.x + 1.0f -startIntersection.intersectionX)*deltaDistX;
		}

		if(ray.getDirection().z > 0)
		{
			stepZ = 1;
			sideDistZ = (currentRow + topLeftPosition.z + 1.0f -startIntersection.intersectionZ)*deltaDistZ;

		}
		else
		{
			stepZ = -1;
			sideDistZ = (startIntersection.intersectionZ-currentRow-topLeftPosition.z)*deltaDistZ;
		}

		while(true)
		{
			nearestIntersection = computeIntersectionTriangleQuad(ray, currentRow, currentColumn);
			if(nearestIntersection.doesIntersect)
			{
				return nearestIntersection;
			}

			if(sideDistX < sideDistZ)
			{
				sideDistX += deltaDistX;
				currentColumn += stepX;
				float height = startIntersection.intersectionY + ray.getDirection().y*sideDistX;
				  if(ray.getDirection().y > 0 && startIntersection.intersectionY > maxHeight)
				  {
					    return nearestIntersection;
				  }

				  if(ray.getDirection().y < 0 && startIntersection.intersectionY < minHeight)
				  {
					  return nearestIntersection;
				  }

			}
			else
			{
				sideDistZ += deltaDistZ;
				currentRow += stepZ;

				float height = startIntersection.intersectionY + ray.getDirection().y*sideDistX;
				if(ray.getDirection().y > 0 && startIntersection.intersectionY > maxHeight)
				 {
					    return nearestIntersection;
				  }

				 if(ray.getDirection().y < 0 && startIntersection.intersectionY < minHeight)
				 {
					  return nearestIntersection;
				 }
			}

			if(currentRow < 0 || currentColumn < 0 || currentRow >= numRows-1|| currentColumn >= numColumns-1)
			{
				return nearestIntersection;
			}
		}

	    return nearestIntersection;
	}
