#include "TestModel.h"
#include <glm/glm.hpp>
#include "BBox.h"
#include <iostream>

using namespace std;
using glm::vec3;
using glm::mat3;


KdNode::KdNode() = default;

KdNode* KdNode::build(vector<Triangle>& tris, int depth) const{
	KdNode* node = new KdNode();
	node->triangles = tris;
	node->left = NULL;
	node->right = NULL;
	node->BBox = BoundingBox(tris);
	// TODO: Write this function
	if (tris.size() == 0) {
		return node;
	}

	if (tris.size() == 1) {
		node->left = new KdNode();
		node->right = new KdNode();
		node->left->triangles = vector<Triangle>();
		node->right->triangles = vector<Triangle>();
		return node;
	}

	//initiate vectors for left and right sides of point
	vector<Triangle> left_tris;
	vector<Triangle> right_tris;

	int axis = getLongestAxis(node->BBox);
	// sort triangles to left or right depending on their midpoint relative to the midpoint on the longest axis
	for (int i = 0; i < tris.size(); i++) {
		switch (axis) {
		case 0:
			node->BBox.midPoint.x >= tris[i].midPoint.x ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case 1:
			node->BBox.midPoint.y >= tris[i].midPoint.y ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case 2:
			node->BBox.midPoint.z >= tris[i].midPoint.z ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		}
	}
	
	// check if one side is empty, this is needed for the next step
	int l_s = left_tris.size();
	int r_s = right_tris.size();
	if (l_s == 0 && r_s > 0) {
		left_tris = right_tris;
	}
	if (r_s == 0 && l_s > 0) {
		right_tris = left_tris;
	}

	// if 50% of triangles match, don't subdivide further
	int matches = 0;
	for (int i = 0; i < left_tris.size(); i++) {
		for (int j = 0; j < right_tris.size(); j++) {
			if (isSameTriangle(left_tris[i], right_tris[j])) {
				matches++;
			}
		}
	}

	if ((float)matches / left_tris.size() < 0.5 && (float)matches / right_tris.size() < 0.5) {
		//less than 50% match, subdivide the tree and build down both sides
		cout << "subdividing tree with " << endl;
		node->left = build(left_tris, depth + 1);
		node->right = build(right_tris, depth + 1);
	}
	else {
		node->left = new KdNode();
		node->right = new KdNode();
		node->left->triangles = vector<Triangle>();
		node->right->triangles = vector<Triangle>();
	}

	return node;
}



bool boxIntersection(BoundingBox& bbox, const vec3& origin, const vec3& dir) {
	// check if a ray intersects a box
	// modified from: 
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	//cout << "declaring intersection variables" << endl;
	//cout << bbox.max.x <<  bbox.min.x << bbox.max.x - bbox.min.x << endl;
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	
	//cout << "dx: " << dx << "dy: " << dy << "dz: " << dz <<" declared" << endl;
	
	tmin = (bbox.min.x - origin.x) / dir.x;
	tmax = (bbox.max.x - origin.x) / dir.x;
	tymin = (bbox.min.y - origin.y) / dir.y;
	tymax = (bbox.max.y - origin.y) / dir.y;

	if (tmin > tmax) swap(tmin, tmax);
	if (tymin > tymax) swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}

	if (tymin > tmin) {
		tmin = tymin;
	}

	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bbox.min.z - origin.z) / dir.z;
	tzmax = (bbox.max.z - origin.z) / dir.z;

	if (tzmin > tzmax) swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}

	if (tzmin > tmin) {
		tmin = tzmin;
	}

	if (tzmax < tmax) {
		tmax = tzmax;
	}

	return true;
}


int getLongestAxis(BoundingBox& bbox) {
	float dx = bbox.max.x - bbox.min.x;
	float dy = bbox.max.y - bbox.min.y;
	float dz = bbox.max.z - bbox.min.z;

	if (dy >= dz && dz > dx) {
		return 1;
	}
	if (dz >= dy && dy > dx) {
		return 2;
	}


	return 0;
}

bool isSameTriangle(Triangle& a, Triangle& b) {
	return (a.v0 == b.v0 && a.v1 == b.v1 && a.v2 == b.v2);
}