#pragma once
#include "TestModel.h"
#include <glm/glm.hpp>
#include <iostream>
using glm::vec3;


using namespace std;


struct BoundingBox {

	vec3 max;
	vec3 min;
	vec3 midPoint;

	BoundingBox() = default;

	BoundingBox(vector<Triangle>& tris) {
		max = vec3(0,0,0);
		max = vec3(0,0,0);
		for (int i = 0; i < tris.size(); ++i) {
			// set max point
			if (tris[i].v0.x > max.x) max.x = tris[i].v0.x;
			if (tris[i].v0.y > max.y) max.y = tris[i].v0.y;
			if (tris[i].v0.z > max.z) max.z = tris[i].v0.z;
			if (tris[i].v1.x > max.x) max.x = tris[i].v1.x;
			if (tris[i].v1.y > max.y) max.y = tris[i].v1.y;
			if (tris[i].v1.z > max.z) max.z = tris[i].v1.z;
			if (tris[i].v2.x > max.x) max.x = tris[i].v2.x;
			if (tris[i].v2.y > max.y) max.y = tris[i].v2.y;
			if (tris[i].v2.z > max.z) max.z = tris[i].v2.z;

			// set min point
			if (tris[i].v0.x < min.x) min.x = tris[i].v0.x;
			if (tris[i].v0.y < min.y) min.y = tris[i].v0.y;
			if (tris[i].v0.z < min.z) min.z = tris[i].v0.z;
			if (tris[i].v1.x < min.x) min.x = tris[i].v1.x;
			if (tris[i].v1.y < min.y) min.y = tris[i].v1.y;
			if (tris[i].v1.z < min.z) min.z = tris[i].v1.z;
			if (tris[i].v2.x < min.x) min.x = tris[i].v2.x;
			if (tris[i].v2.y < min.y) min.y = tris[i].v2.y;
			if (tris[i].v2.z < min.z) min.z = tris[i].v2.z;
		}

		// set mid point
		midPoint = (max - min) / 2.0f;
		
	}
};

class KdNode {
public:
	BoundingBox BBox;
	KdNode* left;
	KdNode* right;
	vector<Triangle> triangles;

	KdNode();

	KdNode* build(vector<Triangle>& tris, int depth) const;

};

bool isSameTriangle(Triangle& a, Triangle& b);

int getLongestAxis(BoundingBox& bbox);

bool boxIntersection(BoundingBox& bbox, const vec3& origin, const vec3& dir);

