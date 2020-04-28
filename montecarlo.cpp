#define _USE_MATH_DEFINES
#include "montecarlo.h"
#include <glm/gtc/random.hpp>
#include <glm/glm.hpp>
#include <math.h>


using namespace std;

/*
	generate a normal vector in the unit hemisphere of a given normal vector
	@param hitPointNormal normal vector for the intersection point


*/
void RandomUnitVectorInHemisphereOf(vec3 & hitPointNormal, vec3& result){
	
	//get random unit vector on a sphere
	randomPointOnSphere(result);

	//make sure it's in the same hemisphere as the normal
	result = result * glm::sign(glm::dot(hitPointNormal, result));
}


/*
	Generate random values on a sphere
	Source: http://mathworld.wolfram.com/SpherePointPicking.html

	@param result: empty vector to put results in
*/
void randomPointOnSphere(vec3 & result)
{
	//cgenerate random angle
	float angle = glm::linearRand(0.0f, 2 * float(M_PI));

	float u = glm::linearRand(-1.0f, 1.0f);

	float u_sqr = u * u;
	float oneMinusUsqr = glm::sqrt(1.0f - u_sqr);

	result.x = oneMinusUsqr * glm::cos(angle);
	result.y = oneMinusUsqr * glm::sin(angle);
	result.z = u;
}


