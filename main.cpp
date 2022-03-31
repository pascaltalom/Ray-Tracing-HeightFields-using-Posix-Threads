#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>

#include <iostream>
#include <cmath>
#include <time.h>

#include "Ray.h"
#include "Plane.h"
#include "Triangle.h"
#include "HeightMap.h"
#include "imageloader.h"

#include <pthread.h>

#define NUM_THREADS 15
pthread_t threads[NUM_THREADS];

///Screen Size
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

float* heights;
int heightMapWidth;
int heightMapHeight;
HeightMap* testHeightMap;

double camera_angle;

float* pixelColors;

struct ThreadParameters
{
    int startRow;
    int endRow;
};

float aspect = SCREEN_HEIGHT/(float)SCREEN_WIDTH;
float alpha = 60*M_PI/180.0f;
float d = 1.0f/tanf(alpha/2.0f);

ThreadParameters threadParameters[NUM_THREADS];

vec3 sphereCenter;
float squaredSphereRadius;

const float DEGREE_TO_RADIAN = 0.01745f;
float cameraUpAngle = -DEGREE_TO_RADIAN*1.0f;
float cameraHorizontalAngle = DEGREE_TO_RADIAN*1.0f;

//Camera Vue Angle
vec3 upVector(sinf(cameraUpAngle)*sinf(cameraHorizontalAngle),cosf(cameraUpAngle),sinf(cameraUpAngle)*cosf(cameraHorizontalAngle));
vec3 frontVector(cosf(cameraUpAngle)*sinf(cameraHorizontalAngle),sinf(cameraUpAngle),cosf(cameraUpAngle)*cosf(cameraHorizontalAngle));
vec3 rightVector(cosf(cameraHorizontalAngle),0,-sinf(cameraHorizontalAngle));

vec3 cameraPosition(0,0,0);


//Pixel Computation
void* computeColors(void* arguments)
{
    ThreadParameters* threadParameters = (ThreadParameters*)arguments;
     for(int y = threadParameters->startRow; y <= threadParameters->endRow; y ++)
     {
        for(int x = 0; x < SCREEN_WIDTH; x ++)
        {
            float screenX = x/((float)SCREEN_WIDTH-1)*2.0-1; //Convert to the range -1 to 1
            float screenY = y/((float)SCREEN_HEIGHT-1)*2.0-1;
            vec3 direction(screenX,screenY*aspect,-d);
            direction = normalize(direction);

            vec3 cameraDirection = rightVector*direction.x + upVector*direction.y + frontVector*direction.z;
            Ray testray(cameraPosition,cameraDirection);

            IntersectionData intersection = testHeightMap->computeIntersection(testray);

            float brightness = 0;
            if(intersection.doesIntersect)
            {
                brightness = intersection.brightness;
            }
            pixelColors[y*SCREEN_WIDTH+x] = brightness;
        }
    }
    return 0;
}

//HeightMap Load
void loadTerrain(float height)
{
    Image* image = loadBMP("64x642.bmp"); //load bmp file
	heights = new float[image->width*image->height];
	heightMapWidth = image->width;
	heightMapHeight = image->height;

	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f));
			heights[y*image->height+x] = h;
		}
	}
	delete image;
}

//Clock time taken for rendering
static long GetTickCountMs()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (long)(ts.tv_nsec / 1000000) + ((long)ts.tv_sec * 1000ull);
}

//keyboard directions
float dt = 0;

bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;

//Display heightmap
void display()
{
	long startMillis = GetTickCountMs();
    //recalculate camera vue after any keyboard movement
	upVector = vec3(sinf(cameraUpAngle)*sinf(cameraHorizontalAngle),cosf(cameraUpAngle),sinf(cameraUpAngle)*cosf(cameraHorizontalAngle));
	frontVector = vec3(cosf(cameraUpAngle)*sinf(cameraHorizontalAngle),sinf(cameraUpAngle),cosf(cameraUpAngle)*cosf(cameraHorizontalAngle));
	rightVector = vec3(cosf(cameraHorizontalAngle),0,-sinf(cameraHorizontalAngle));

	if(upPressed)
	{
		cameraPosition += frontVector*1.5f*dt;
	}
	if(downPressed)
	{
		cameraPosition -= frontVector*1.5f*dt;
	}

	if(leftPressed)
	{
		cameraHorizontalAngle -= 1.1f*dt;
	}

	if(rightPressed )
	{
		cameraHorizontalAngle += 1.1f*dt;
	}

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPointSize(1);
    glBegin(GL_POINTS);

    Triangle testTriangle(vec3(0,1.0,-10),vec3(-1.0,0,-10),vec3(1.0,0,-10));

    int numRowsPerThread = SCREEN_HEIGHT/NUM_THREADS;
    for(int i = 0; i < NUM_THREADS; i++)
    {
        threadParameters[i].startRow = i*numRowsPerThread;
        threadParameters[i].endRow = threadParameters[i].startRow + numRowsPerThread-1;

        pthread_create(&threads[i],0,computeColors,&threadParameters[i]);
    }

    //Wait for all threads to be finished
    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i],0);
    }

    for(int x = 0; x < SCREEN_WIDTH; x ++)
    {
        for(int y = 0; y < SCREEN_HEIGHT; y ++)
        {
            float brightness = pixelColors[y*SCREEN_WIDTH+x];
            glColor3f(0, brightness, 0);
            glVertex2i(x,y);
        }
    }
    glEnd();
    glutSwapBuffers();
    glutPostRedisplay();

    long passedMillis = GetTickCountMs() - startMillis;

    dt = passedMillis/1000.0f;
    //std::cout << "Time: " << dt << std::endl;
}

void keyBoardFunction(unsigned char key, int mouseX, int mouseY)
{

	if(key == 'd')
	{
		rightPressed  = true;
	}

	if(key == 'a')
	{
		leftPressed  = true;
	}

	if(key == 's')
	{
		upPressed = true;
	}
	if(key == 'w')
	{
		downPressed = true;
	}
}

void keyBoardUpFunction(unsigned char key, int mouseX, int mouseY)
{
	if(key == 'd')
	{
		rightPressed = false;
	}

	if(key == 'a')
	{
		leftPressed = false;
	}

	if(key == 's')
	{
		upPressed = false;
	}
	if(key == 'w')
	{
		downPressed = false;
	}

}

int main(int argc, char** argv)
{
    loadTerrain(2.0);
    //TopLeft X, TopRight Y, TopLeftZ, diameterQuad, Rows,cols, heights
    testHeightMap = new HeightMap(-4,-4,-80,1.0,heightMapHeight,heightMapWidth,heights);
    pixelColors = new float[SCREEN_WIDTH*SCREEN_HEIGHT];

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Ray Traced Height Fields");
    glutDisplayFunc(display);

    glutKeyboardFunc(keyBoardFunction);
    glutKeyboardUpFunc(keyBoardUpFunction);


    glutMainLoop();

    delete[] pixelColors;
    pixelColors = 0;

    delete testHeightMap;
    testHeightMap = 0;

    delete[] heights;
    heights = 0;

    return 0;
}
