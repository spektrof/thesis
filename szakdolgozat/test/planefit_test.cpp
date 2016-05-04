#include <iostream>
#include <set>
#include "c:\OGLPack\include\glm\glm.hpp"
#include "../Engine/vectors.h"
#include "../Engine/diffvec.h"
#include <fstream>

namespace approx
{
	//copyzva lett conversion.h bol
	template <class T> Vector3<T> convert(const glm::vec3& v) {
		return{ v.x, v.y, v.z };
	}
	//copyzva lett conversion.h bol
	template <class T> glm::vec3 convert(const Vector3<T>& p) {
		return glm::vec3((float)p.x, (float)p.y, (float)p.z);
	}
	template <class T>
	std::ostream& operator << (std::ostream& out, const Vector3<T>& v)
	{
			out << v.x << " " << v.y << " " <<v.z<<"\n";
			return out;
	}
	
};

/*Copyzva lett ez a namespace resz*/
namespace Utility{
	struct PlaneResult {
		approx::Vector3<float> normal;
		approx::Vector3<float> point;
		PlaneResult(const approx::Vector3<float> n = approx::Vector3<float>(1,0,0) , const approx::Vector3<float>& p = approx::Vector3<float>(0,0,0) ) : normal(n), point(p) {}
		friend std::ostream& operator << (std::ostream& out, const PlaneResult& pr)
		{
			out << "Normal: "<<pr.normal <<"\nPoint: " << pr.point<<"\n";
			return out;
		}
	};
};


#define ONLYFORTEST 1
#include "../Strategy/planegetters.h"

int main()
{
	std::set<approx::Vector3<float>, approx::DifferentVector3<float>> points;
	points.insert(approx::Vector3<float>(-1,-1,-1));
	points.insert(approx::Vector3<float>(-1,-1,1));
	points.insert(approx::Vector3<float>(-1,1,-1));
	points.insert(approx::Vector3<float>(-1,1,1));
	
	PlaneGetterFunctions<int>* OnlyForPlFit = new PlaneGetterFunctions<int>(std::vector<std::vector<int>>());
	
	bool ok=true,cop=false;
	Utility::PlaneResult res = OnlyForPlFit->PlanarFittingOn3dPoints(points,ok,cop);
	std::cout << res <<"\n";
	return 0;
}
