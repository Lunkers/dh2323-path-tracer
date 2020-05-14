#pragma once
#include <glm/glm.hpp>
using glm::vec3;

void RandomUnitVectorInHemisphereOf( vec3& hitPointNormal, vec3& result);

void randomPointOnSphere(vec3& result, vec3& normal);

void createCoordinateSystem(const vec3 & N, vec3 & Nt, vec3 & Nb);

vec3 uniformSampleHemisphere(const float & u1, const float & u2);
