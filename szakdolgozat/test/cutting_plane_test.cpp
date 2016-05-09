/*Keszitette: Lukacs Peter
	A sikrajzoli forgatasi matrixokra par teszteset.
	Kimeneten latjuk a kivant vektort, az elforgatas szoget, tengelyet, a forgatasi matrixot es az ellenorzest
*/
#include <iostream>

#define ONLYFORTEST 1

#define M_PI    3.14159265358979323846264338327950288 
#include "c:/OGLPack/include/glm/glm.hpp"
#include "c:/OGLPack/include/glm/gtc/matrix_transform.hpp"
#include "c:/OGLPack/include/glm/gtx/transform2.hpp"
#include "../Engine/face.h"
	
#include <fstream>
#include <random>

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

#include "../Utils/Utility.h"
#include "../Utils/Utility.cpp"

void GetTest(const glm::vec3& v,const char* txt);

 int main()
 {
	std::random_device rd;
	glm::vec3 Same = glm::vec3(0,0,1);
	glm::vec3 Opposite = glm::vec3(0,0,-1);
	glm::vec3 OtherAxis = glm::vec3(0,1,0);
	glm::vec3 Null = glm::vec3(0,0,0); //hibas bemenet
	glm::vec3 Random = glm::normalize(glm::vec3(rd()%10,rd()%10,rd()%10));
	glm::vec3 Random2 = glm::normalize(glm::vec3(rd()%10,rd()%10,rd()%10));
	 
	GetTest(Same,"A default normalissal megegyezo: ");
	GetTest(Opposite, "Ellentetes iranyba nezo vektor: ");
	GetTest(OtherAxis, "Masik tengely fele mutato vektor: ");
	GetTest(Null,"Nullvektor(hibas bemenet - dokumentaciot serti!): ");
	GetTest(Random, "Veletlen: ");
	GetTest(Random2, "Veletlen: ");
	
	system("pause");
	return 0;
 }
 
 void GetTest(const glm::vec3& v,const char* txt)
 {
	std::cout<< txt<< v;
	glm::mat4 res = Utility::GetRotateFromNorm(v);
	std::cout<<"Forgatasi matrix \n" << res <<"\n";
	std::cout<< "Ellenorzes: "<< res *  glm::vec4(0,0,1,0) <<"\n-------------\n" ;
 }