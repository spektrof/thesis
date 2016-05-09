/* Keszitette: Lukacs Peter
	Szomszedsagi matrix felepito tesztelese
*/

#include <iostream>
#include <vector>
#define M_PI 3.14159265358979323846264338327950288 
#include "c:\OGLPack\include\glm\glm.hpp"
#include "c:\OGLPack\include\glm\gtc\matrix_transform.hpp"
#include "c:\OGLPack\include\glm\gtx\transform2.hpp"

#define APPROX_NO_CONVERSION
#include "../Engine/approximator.h"

std::ostream& operator << (std::ostream& out, const glm::vec3& v)
{
	out<< v.x << " " << v.y << " " << v.z<<"\n";
	return out;
} 

std::ostream& operator << (std::ostream& out, const std::vector<std::vector<int>>& res)
{
	for (std::vector<std::vector<int>>::const_iterator it = res.begin(); it != res.end(); ++it)
	{
		for (std::vector<int>::const_iterator jt = it->begin(); jt != it ->end(); ++jt)
		{
			out << *jt <<" ";
		}
		out <<"\n";
	}
	return out;
}

#define ONLYFORTEST 1
#include "../Utils/Utility.h"
#include "../Utils/Utility.cpp"

int main()
{
	std::vector<approx::Face<float>>* F = new std::vector<approx::Face<float>>();
	
	std::cout<<"Ures face vektor eseten:\n";
	std::vector< std::vector<int>> res = Utility::GetAdjacencyMatrix(F);
	std::cout << res <<"\n";
	
	std::cout << "Kristaly eseten:\n";
	approx::Approximator<float> app;
	if(app.set_target("../Targets/kri.obj",2.0f))
	{
		res = Utility::GetAdjacencyMatrix(&(app.target().face_container()));
		std::cout<<res<<"\n";
	}
	else
	{
		std::cout<<"Cant open that file\n";
	}

	system("pause");
	return 0;
}
