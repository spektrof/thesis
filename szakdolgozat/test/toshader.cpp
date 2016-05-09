#include <iostream>


#define M_PI    3.14159265358979323846264338327950288 
#include "c:/OGLPack/include/glm/glm.hpp"
#include "c:/OGLPack/include/glm/gtc/matrix_transform.hpp"
#include "c:/OGLPack/include/glm/gtx/transform2.hpp"
std::ostream& operator << (std::ostream& out, const glm::vec3& v)
{
	out<< v.x << " " << v.y << " " << v.z<<"\n";
	return out;
} 

 std::ostream& operator << (std::ostream& out, const glm::vec4& v)
{
	out<< v.x << " " << v.y << " " << v.z<< " " << v.a <<"\n";
	return out;
} 

 std::ostream& operator << (std::ostream& out, const glm::mat4& m)
{
	for (int i=0; i<4;++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			out << m[i][j]<<" " ;
		}
		out<<"\n";
	}
	return out;
}

int main()
{
	glm::vec3 gl = glm::vec3(1,1,-1);
	glm::mat4 V = glm::lookAt(glm::vec3(1,0,0),glm::vec3(1,1,0),glm::vec3(0,0,1));
	glm::mat4 M = glm::translate<float>(-1,-1,-1);
	glm::mat4 M2 = glm::mat4(1.0f);
	
	glm::mat4 P = glm::perspective(45.0f, 800 / (float)600, 0.01f, 1000.0f);
	
	std::cout << M * glm::vec4(gl, 1) << "\n";
	std::cout << V * M * glm::vec4(gl, 1) << "\n";
	std::cout << P * V * M * glm::vec4(gl, 1) << "\n";
	
	std::cout << M2 * glm::vec4(gl, 1) << "\n";
	std::cout << V * M2 * glm::vec4(gl, 1) << "\n";
	std::cout << P * V * M2 * glm::vec4(gl, 1) << "\n";
	
	return 0;
}