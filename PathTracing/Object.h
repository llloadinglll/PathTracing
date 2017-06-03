#pragma once
#include <vector>
#include "Material.h"
#include <Eigen/StdVector>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
#include <float.h>

using namespace Eigen;
using namespace std;

#define USE_BOUNDING_BOX 1

class Object;

typedef struct {
	bool is_intersected;
	Object* obj;
	Vector3f pos;
	Vector3f normal;
	double min_t;
}Intersection;

enum ObjectType {
	SPHERE,MESH
};

class Object
{
public:
	Object():min_x(FLT_MAX),max_x(-FLT_MAX), min_y(FLT_MAX), max_y(-FLT_MAX), min_z(FLT_MAX), max_z(-FLT_MAX){}
	Material* material;
	vector<vector<int> > faces;
	void setLimits(Vector3f v);
	void setBoundingBox(vector<Vector3f>& vertexs);
	virtual void intersect(Intersection& inter,const vector<Vector3f>& vertexs, Vector3f pos, Vector3f dir);
	enum ObjectType type;
protected:
	float min_x, max_x, min_y, max_y, min_z, max_z;
	bool have_bounding_box = false;
	vector<vector<int> > bounding_box;
	bool intersectMesh(Intersection& inter, const vector<vector<int> >& mesh, const vector<Vector3f>& vertexs, Vector3f pos, Vector3f dir, bool is_bounding_box=false);
	bool intersectTriangle(const Vector3f& orig, const Vector3f& dir, const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float* t, float* u, float* v);
	inline Vector3f caculateNormal(const Vector3f& a, const Vector3f& b, const Vector3f& c);
	
};

