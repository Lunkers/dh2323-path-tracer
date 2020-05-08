#include "TestModel.h"
#include <glm/glm.hpp>
#include "BBox.h"

using namespace std;
using glm::vec3;
using glm::mat3;


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

bool hit(KdNode* node, const vec3& origin, vec3& dir, Intersection& closestIntersection) {
	 //recursive search for intersections
	if (boxIntersection(node->BBox, origin, dir)) {
		vec3 normal;
		float m = std::numeric_limits<float>::max();
		float t, u, v;
		vec3 v0, v1, v2, e1, e2, b, x;
		mat3 A(float(-1) * dir, e1, e2);

		//if either child still has triangles, go down both sides recursively
		if (node->left->triangles.size() > 0 || node->right->triangles.size() > 0) {
			bool hitLeft = hit(node->left, origin, dir, closestIntersection);
			bool hitRight = hit(node->right, origin, dir, closestIntersection);
			return hitLeft || hitRight;
		}

		else {
			// we have reached a leaf
			for (int i = 0; i < node->triangles.size(); i++) {
				//check for intersections here
				
				v0 = node->triangles[i].v0;
				v1 = node->triangles[i].v1;
				v2 = node->triangles[i].v2;
				e1 = v1 - v0;
				e2 = v2 - v0;
				b = origin - v0;
				A = mat3(-dir, e1, e2);
				x = glm::inverse(A) * b;
				t = x[0];
				u = x[1];
				v = x[2];
				if (u >= 0 && v >= 0 && u + v <= 1 && t >= 0) {

					vec3 intPoint = v0 + u * e1 + v * e2;
					float dist = glm::distance(intPoint, origin);
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
	}
	return false;

}

bool boxIntersection(BoundingBox& bbox, const vec3& origin, const vec3& dir) {
	// check if a ray intersects a box
	// modified from: 
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	float dx = bbox.max.x - bbox.min.x;
	float dy = bbox.max.y - bbox.min.y;
	float dz = bbox.max.z - bbox.min.z;
	tmin = (bbox.min.x - origin.x) / dx;
	tmax = (bbox.max.x - origin.x) / dx;
	tymin = (bbox.min.y - origin.y) / dy;
	tymax = (bbox.max.y - origin.y) / dy;

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

	tzmin = (bbox.min.z - origin.z);
	tzmax = (bbox.max.z - origin.z);

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
	//find longest axis of a bounding box	
	return 0;
}

bool isSameTriangle(Triangle& a, Triangle& b) {
	return (a.v0 == b.v0 && a.v1 == b.v1 && a.v2 == b.v2);
}