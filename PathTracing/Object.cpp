#include "Object.h"

void Object::setLimits(Vector3f v) {
	if (v.x() < min_x) min_x = v.x();
	if (v.x() > max_x) max_x = v.x();
	if (v.y() < min_y) min_y = v.y();
	if (v.y() > max_y) max_y = v.y();
	if (v.z() < min_z) min_z = v.z();
	if (v.z() > max_z) max_z = v.z();
}


void Object::setBoundingBox(vector<Vector3f>& vertexs) {

	if (faces.size() < 20) {
		have_bounding_box = false;
		return;
	}
	have_bounding_box = true;

	int t = vertexs.size();
	vertexs.push_back(Vector3f(min_x, min_y, min_z));
	vertexs.push_back(Vector3f(max_x, min_y, min_z));
	vertexs.push_back(Vector3f(min_x, max_y, min_z));
	vertexs.push_back(Vector3f(max_x, max_y, min_z));
	vertexs.push_back(Vector3f(min_x, min_y, max_z));
	vertexs.push_back(Vector3f(max_x, min_y, max_z));
	vertexs.push_back(Vector3f(min_x, max_y, max_z));
	vertexs.push_back(Vector3f(max_x, max_y, max_z));

	bounding_box.clear();
	int f[12][3] = {
		{0,1,2},{1,2,3},
		{0,1,4},{1,4,5},
		{2,3,6},{3,6,7},
		{0,2,4},{2,4,6},
		{1,3,5},{3,5,7},
		{4,5,6},{5,6,7}
	};
	for (int i = 0; i < 12; i++) {
		vector<int> tt;
		tt.push_back(t + f[i][0]);
		tt.push_back(t + f[i][1]);
		tt.push_back(t + f[i][2]);
		bounding_box.push_back(tt);
	}
}

void Object::intersect(Intersection& inter, const vector<Vector3f>& vertexs, Vector3f pos, Vector3f dir) {
#ifdef USE_BOUNDING_BOX
    if (have_bounding_box && !intersectMesh(inter, bounding_box, vertexs, pos, dir, true))
			return;
#endif
	intersectMesh(inter, faces, vertexs, pos, dir, false);
}

inline Vector3f Object::caculateNormal(const Vector3f& a, const Vector3f& b, const Vector3f& c) {
	Vector3f ab = b - a;
	Vector3f ac = c - a;
	return ab.cross(ac).normalized();
}

bool Object::intersectMesh(Intersection& inter, const vector<vector<int> >& mesh, const vector<Vector3f>& vertexs, Vector3f pos, Vector3f dir, bool is_bounding_box) {
	for (int i = 0; i < mesh.size(); i++) {
		const vector<int>& vi = mesh[i];
		int vs = vi.size();
		if (vs < 3 || vs>4) printf("[intersectMesh] face vertexs number error: %d vertexs. only support 3 or 4 vertexs now.", vs);

		bool b;
		float t, u, v;
		b = intersectTriangle(pos, dir, vertexs[vi[0]], vertexs[vi[1]], vertexs[vi[2]], &t, &u, &v);
		if (!b && vs==4)
			b= intersectTriangle(pos, dir, vertexs[vi[0]], vertexs[vi[2]], vertexs[vi[3]], &t, &u, &v);

		if (!is_bounding_box) {
			if (b && t > 1e-4  && t < inter.min_t) {
				inter.is_intersected = true;
				inter.min_t = t;
				inter.pos = pos + t * dir;
				inter.normal = caculateNormal(vertexs[vi[0]], vertexs[vi[1]], vertexs[vi[2]]);
				inter.obj = this;
			}
		}
		else if (b && t > 1e-4)
		{
			return true;
		}
	}
	return false;
}

// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection
//http://www.cnblogs.com/samen168/p/5162337.html
bool Object::intersectTriangle(const Vector3f& orig, const Vector3f& dir,
	const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
	float* t, float* u, float* v)
{
	// E1
	Vector3f E1 = v1 - v0;

	// E2
	Vector3f E2 = v2 - v0;

	// P
	Vector3f P = dir.cross(E2);

	// determinant
	float det = E1.dot(P);

	// keep det > 0, modify T accordingly
	Vector3f T;
	if (det >0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;

	// Calculate u and make sure u <= 1
	*u = T.dot(P);
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	Vector3f Q = T.cross(E1);

	// Calculate v and make sure u + v <= 1
	*v = dir.dot(Q);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = E2.dot(Q);

	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}
