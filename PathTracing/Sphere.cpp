#include "Sphere.h"



Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}


void Sphere::intersect(Intersection& inter, const vector<Vector3f>& vertexs, Vector3f pos, Vector3f dir){
	Vector3f PR = center - pos;
	dir.normalize();
	double t = PR.dot(dir);
	double dist = sqrt(PR.squaredNorm() - t*t);
	if (dist >= radius) return ;
	
	double delta = sqrt(radius*radius - dist*dist);
	double t1 = t - delta ;
	double t2 = t + delta;

	if (t1 > 1e-4 && t1 < inter.min_t) {
		inter.is_intersected = true;
		inter.min_t = t1;
		inter.obj = this;
		inter.pos = pos + t1 *dir;
#ifndef NDEBUG
		if (abs((inter.pos - center).norm() - radius) > 1e-4)
			cout << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Sphere::intersect error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
#endif
		inter.normal = (inter.pos - center).normalized();
	}
	else if (t1 < 1e-4 && t2>1e-4 && t2 < inter.min_t)
	{
		inter.is_intersected = true;
		inter.min_t = t2;
		inter.obj = this;
		inter.pos = pos + t2 *dir;
		inter.normal = (center - inter.pos).normalized();
	}
}