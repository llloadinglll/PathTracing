#pragma once

#include <string>
#include <Eigen/StdVector>

using namespace Eigen;

class Material
{
public:
	std::string name;
	Vector3f Ka, Kd,Ks;
	float Ns;
	float Tr=0, Ni=1;
	int illum;
	bool is_light;
};

