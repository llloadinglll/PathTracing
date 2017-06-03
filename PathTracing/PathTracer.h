#pragma once

#define _USE_MATH_DEFINES
#define USE_LIGHT_NAME 0

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "Material.h"
#include <Eigen/StdVector>
#include "Camera.h"
#include <time.h>
#include "Object.h"
#include "MathFunc.h"
#include "GL/glut.h"
#include <iomanip>
#include "Sphere.h"

using namespace std;
using namespace Eigen;

void display(void);

class PathTracer
{
public:
	PathTracer();
	~PathTracer();
	bool loadParameterFile(string name);
	bool loadParameters(string name);
	bool loadObject(string name);
	unordered_map<string, Material*> Materials;
	vector<Object*>  objects;
	vector<Vector3f>  vertexs;
	void setCamera(Vector3f pos, Vector3f front, Vector3f up);
	void setPixelSize(int width, int height);
	void render();
	Vector3f env_color=Vector3f(1, 1, 1);
	float env_intensity = 3;
	Vector3f light_color = Vector3f(1, 1, 1);
	float light_intensity = 20;
	float fov = 60.0f;
	int sample_num = 30;
	Vector3f pathTracing(Vector3f pos, Vector3f dir, int depth, bool inside = false);
	void intersect(Intersection & inter, Vector3f pos, Vector3f dir);
	Vector3f randomReflectedRay(const Vector3f & n, double given_phi = 0) const;
	Vector3f weightedRandomReflectedRay(const Vector3f& v, const Vector3f &n, const Material* m) const;
	Vector3f refractedRay(const Vector3f& V, const Vector3f &N, float Ni, bool& inside)const;
	string light_name = "blinn2SG";
	int max_depth;
	void glInit(const char*);
	static int width, height;
	static GLubyte* frame;
	inline void setColor(int x, int y, Vector3f color);
private:
	void loadMaterial(string name);
	void calculatePixel();

	Camera camera;

	float near_clip_z = 0.1f;
	Vector3f pixel_origin_pos;
	Vector3f pixel_x_inc;
	Vector3f pixel_y_inc;
};

