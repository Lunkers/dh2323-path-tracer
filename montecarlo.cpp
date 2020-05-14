#define _USE_MATH_DEFINES
#include "montecarlo.h"
#include <glm/glm.hpp>
#include <math.h>
#include <random>


using namespace std;

default_random_engine factory;
normal_distribution<float> dist(0.0, 1.0);
uniform_real_distribution<float> uniDist(0, 1);
/*
	generate a normal vector in the unit hemisphere of a given normal vector
	@param hitPointNormal normal vector for the intersection point


*/
void RandomUnitVectorInHemisphereOf(vec3 & hitPointNormal, vec3& result){
	vec3 Nt, Nb;
	createCoordinateSystem(hitPointNormal, Nt, Nb);
	float u1 = uniDist(factory);
	float u2 = uniDist(factory);

	vec3 sample = uniformSampleHemisphere(u1, u2);

	//transform sample to normal coordinate space
	result = vec3(sample.x * Nb.x + sample.y * hitPointNormal.x + sample.z * Nt.x,
		sample.x * Nb.y + sample.y * hitPointNormal.y + sample.z * Nt.y,
		sample.x * Nb.z + sample.y * hitPointNormal.z + sample.z * Nt.z);

	//get random unit vector on a sphere
	//randomPointOnSphere(result, hitPointNormal);

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
/*
	Generate a coordinate system with the normal as "up" vector
*/
void createCoordinateSystem(const vec3& N, vec3& Nt, vec3& Nb) {
	if (fabs(N.x) > fabs(N.y)) {
		Nt = vec3(N.z, 0, -N.x) / glm::sqrt(N.x * N.x + N.z * N.z);
	}
	else {
		Nt = vec3(0, -N.z, N.y) / glm::sqrt(N.y * N.y + N.z * N.z);
	}

	Nb = glm::cross(N, Nt);
}

vec3 uniformSampleHemisphere(const float& u1, const float& u2) {
	float sinTheta = glm::sqrt(1 - u1 * u1);
	float phi = 2 * M_PI * u2;
	float x = sinTheta * glm::cos(phi);
	float z = sinTheta * glm::sin(phi);

	return vec3(x, u1, z);
}


