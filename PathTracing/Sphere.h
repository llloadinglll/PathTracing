#pragma once

#include "Object.h"

class Sphere : public Object
{
public:
	Sphere();
	~Sphere();
	Vector3f center;
	float radius;
	virtual void intersect(Intersection& inter, const vector<Vector3f>& vertexs, Vector3f pos, Vector3f dir);
protected:

};

