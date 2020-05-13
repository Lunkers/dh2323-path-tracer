#define _USE_MATH_DEFINES
#include "montecarlo.h"
#include <glm/glm.hpp>
#include <math.h>
#include <random>


using namespace std;

default_random_engine factory;
normal_distribution<float> dist(0.0, 1.0);

/*
	generate a normal vector in the unit hemisphere of a given normal vector
	@param hitPointNormal normal vector for the intersection point


*/
void RandomUnitVectorInHemisphereOf(vec3 & hitPointNormal, vec3& result){
	
	//get random unit vector on a sphere
	randomPointOnSphere(result, hitPointNormal);

}


/*
	Generate random values on a sphere
	Samples a random value from a normal distribution on each axis, returns a normal vector

	@param normal: normal vector from point
	@param result: empty vector to put results in
*/
void randomPointOnSphere(vec3 & result, vec3& normal)
{

	result.x = dist(factory);
	result.y = dist(factory);
	result.z = dist(factory);

	result = glm::normalize(result);

	if (glm::dot(normal, result) < 0) {
		result = -result;
	}

}


