#define _USE_MATH_DEFINES
#include <iostream>
#include <glm/glm.hpp>
#include "SDL.h"
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "montecarlo.h"
#include <math.h>
#include <thread>

using namespace std;
using glm::vec3;
using glm::mat3;

struct Intersection {
	vec3 position;
	float distance;
	int triangleIndex;
};

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

int NUM_THREADS = 10;

// NB: These values MUST be a multiple of NUM_threads, otherwise the threading won't work!
const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;

//rotation variables
mat3 R = mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
float yaw = 0.2;

// camera variables
float focalLength = (SCREEN_HEIGHT / 2) / 0.71; //assuming 90 degree viewing angle
vec3 cameraPos(0, 0, -2.25);

// xxxtentacion variables
vec3 lightPos(0, -0.5, -0.7);
vec3 lightColor = 14.f * vec3(1, 1, 1);
vec3 indirectLight = 0.5f * vec3(1, 1, 1);


int maxCount = 3;
int numSamples = 100;


float rendered[SCREEN_HEIGHT][SCREEN_WIDTH][3];
// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);
vec3 DirectLight(Intersection& i);
vec3 TracePath(vec3 startPoint, vec3 direction, int depthCount);
void threadDraw(int x, int y);

int main(int argc, char* argv[])
{
	//load cornell box model
	LoadTestModel(triangles);
	//kd = new KdNode();
	cout << triangles.size() << endl;
	//kd = kd->build(triangles, 1);
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	

	t = SDL_GetTicks();	// Set start value for timer.
	Draw();
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	float dh = dt / 1000;
	cout << "Render time: " << dt << " ms." << " - " << dh << "s." << endl;

	while (NoQuitMessageSDL())
	{
		Update();

	}

	SDL_SaveBMP(screen, "screenfgfgshot.bmp");
	return 0;
}

void Update()
{
	// Compute frame time:

	vec3 forward(R[2][0], R[2][1], R[2][2]);
	vec3 right(R[0][0], R[0][1], R[0][2]);

	Uint8* keystate = SDL_GetKeyState(0);
	if (keystate[SDLK_UP]) {
		// Move camera forward
		cameraPos = cameraPos + 0.2f * forward;

	}
	if (keystate[SDLK_DOWN]) {
		cameraPos = cameraPos - 0.2f * forward;
	}
	if (keystate[SDLK_LEFT]) {
		// Move camera to the left
		yaw = yaw + 0.2;
		R = mat3(vec3(glm::cos(yaw), float(0), -glm::sin(yaw)), vec3(0, 1, 0), vec3(glm::sin(yaw), float(0), glm::cos(yaw)));
		cameraPos = cameraPos - 0.2f * right;
	}
	if (keystate[SDLK_RIGHT]) {
		// Move camera to the right
		yaw = yaw - 0.2;
		R = mat3(vec3(glm::cos(yaw), float(0), -glm::sin(yaw)), vec3(0, 1, 0), vec3(glm::sin(yaw), float(0), glm::cos(yaw)));
		cameraPos = cameraPos + 0.2f * right;
	}
	if (keystate[SDLK_w]) {
		lightPos.z += 0.1;
	}
	if (keystate[SDLK_s]) {
		lightPos.z -= 0.1;
	}
	if (keystate[SDLK_d]) {
		lightPos.x += 0.1;
	}
	if (keystate[SDLK_a]) {
		lightPos.x -= 0.1;
	}
	if (keystate[SDLK_q]) {
		lightPos.y += 0.1;
	}
	if (keystate[SDLK_e]) {
		lightPos.y -= 0.1;
	}
}

void Draw()
{
	
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; x+=NUM_THREADS)
		{
			//initialize threads
			vector<thread*> threads;
			for (int k = 0; k < NUM_THREADS; k++) {
				//start rendering thread
				threads.push_back(new thread(threadDraw, x + k, y));
			}

			// block main thread from doing anything until all rays are finished
			for (int l = 0; l < NUM_THREADS; l++) {
				// wait for thread to finish
				threads[l]->join();
				
				//remove the thread
				delete(threads[l]);
			}

			
		}
		cout << "finished row " << y << endl;
		// image has finished rendering, put on screen
		
	}
	cout << "finished rendering, putting on screen!" << endl;
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			vec3 color(rendered[y][x][0], rendered[y][x][1], rendered[y][x][2]);
			PutPixelSDL(screen, x, y, color);
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

// multi-threaded drawing
void threadDraw(int x, int y) {
	Intersection intersection;
	vec3 color;
	vec3 d(float(x) - (SCREEN_WIDTH / 2), float(y) - (SCREEN_HEIGHT / 2), focalLength); //vår ray
	d = R * d;
	for (int i = 0; i < numSamples; ++i) {
		color += TracePath(cameraPos, d, 0); //DO PATH TRACE YO
	}
	color /= numSamples;
	rendered[y][x][0] = color.r;
	rendered[y][x][1] = color.g;
	rendered[y][x][2] = color.b;
	
}

vec3 TracePath(vec3 startPoint, vec3 direction, int depthCount) {

	if (depthCount >= maxCount) {
		return vec3(0, 0, 0); //MÅLA SVART
	}
	Intersection intersection;
	bool hitSomething = ClosestIntersection(startPoint, direction, triangles, intersection);
	if (hitSomething == false) {
		return vec3(0, 0, 0); //MÅLA SVART SLÅ IHOP DESSA TVÅ???????
	}

	vec3 newRay;

	//calculate direct the light emitted from this point
	vec3 emittance = DirectLight(intersection);
	//cout << "the light is " << emittance.r << emittance.g <<  emittance.b << endl;
	const float p = 1 / (2 * M_PI);

	Triangle triangle = triangles[intersection.triangleIndex];
	vec3 normalWhereObjectWasHit = triangle.normal;

	RandomUnitVectorInHemisphereOf(normalWhereObjectWasHit, newRay);
	//float spec = pow(glm::max(glm::dot(direction, newRay), 0.0f), 32);
	// ObjectFactoryContainerFactory NewObjectFactoryContainerFactory = new ObjectFactoryContainer();
	float cos_theta = glm::dot(newRay, normalWhereObjectWasHit);
	float BRDF = triangle.material.specularExponent; //välj vettigt värde



	// Recursively trace reflected light sources.
	vec3 incoming = TracePath(intersection.position, newRay, depthCount + 1);


	// Apply the Rendering Equation here.
	return emittance + (BRDF * incoming * cos_theta / p);
}



bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection) {
	float m = std::numeric_limits<float>::max();
	float t, u, v;
	vec3 v0, v1, v2, e1, e2, b, x;
	mat3 A(float(-1) * dir, e1, e2);
	for (int i = 0; i < triangles.size(); i++) {
		// loop through triangles and solve interesction system
		v0 = triangles[i].v0;
		v1 = triangles[i].v1;
		v2 = triangles[i].v2;
		e1 = v1 - v0;
		e2 = v2 - v0;
		b = start - v0;
		A = mat3(-dir, e1, e2);
		x = glm::inverse(A) * b;
		t = x[0];
		u = x[1];
		v = x[2];
		// check if ray intersects triangle
		if (u >= 0 && v >= 0 && u + v <= 1 && t >= 0) {

			vec3 intPoint = v0 + u * e1 + v * e2;
			float dist = glm::distance(intPoint, start);
			if (dist < m) {
				m = dist;
				closestIntersection = Intersection{ intPoint, dist, i };
			}
		}
	}
	if (m < std::numeric_limits<float>::max()) {
		return true;
	}
	return false;
}

vec3 DirectLight(Intersection& i) {
	Intersection intersection;
	Triangle t =triangles[i.triangleIndex];
	vec3 n = t.normal;
	vec3 r = lightPos - i.position;
	vec3 rHat = glm::normalize(r);
	vec3 D;
	//move origin sligthtly along normal to avoid intersecting with the triangle itself
	vec3 shadowOrig = glm::length(rHat * n) < 0.f ? i.position - n * 0.001f : i.position + n * 0.001f;
	
	if (ClosestIntersection(shadowOrig,rHat, triangles, intersection) ) {
		if (intersection.distance < glm::length(r)) {
			D = vec3(0, 0, 0);
			return D;
		}
		
	}
	D = vec3(lightColor * glm::max(glm::dot(rHat, n), 0.f)) / (4.f * glm::pow(glm::length(r), 2.f) * float(M_PI)); // glm does not seem to include pi for some reason
	D = t.color * D;

	

	return D;
}
