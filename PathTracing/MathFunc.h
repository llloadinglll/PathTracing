#pragma once

#include <Eigen/StdVector>

using namespace std;
using namespace Eigen;

namespace ives {
	inline double rand() {
		return std::rand()*1.0 / RAND_MAX;
	}
	inline Vector3f rgbMultiple( const Vector3f& c1, const Vector3f& c2) {
		return Vector3f(c1.x()*c2.x(), c1.y()*c2.y(), c1.z()*c2.z());
	}
	inline string outVec(const Vector3f& v) {
		return string (" (" +  to_string(v[0]) + "," + to_string(v[1]) + "," + to_string(v[2]) + ") ");
	}
}
