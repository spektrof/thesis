#include <iostream>
#include <set>
#include "c:\OGLPack\include\glm\glm.hpp"
#include "../Engine/vectors.h"
#include "../Engine/diffvec.h"

/*Copyzva lett ez a namespace resz*/
namespace Utility{
	struct PlaneResult {
		approx::Vector3<float> normal;
		approx::Vector3<float> point;
		PlaneResult(const approx::Vector3<float> n = approx::Vector3<float>(1,0,0) , const approx::Vector3<float>& p = approx::Vector3<float>(0,0,0) ) : normal(n), point(p) {}
	};
};

#define ONLYFORTEST 1
#include "../Strategy/planegetters.h"
#include <fstream>

std::ostream& operator << (std::ostream& out,const PlaneGetterFunctions<int>::LuMatricies& LU)
{
	for (int i= 0; i < 3;++i)
	{
		
		out<< LU.L[i][0] << " " <<  LU.L[i][1] << " " <<  LU.L[i][2] << "     "
		   <<  LU.U[i][0] << " " <<  LU.U[i][1] << " " <<  LU.U[i][2] << "\n";
	}
	out<<"P\n";
	for (int i= 0; i < 3;++i)
	{
			out<< LU.P[i][0] << " " <<  LU.P[i][1] << " " <<  LU.P[i][2] << "\n";
	}
	return out;
}

std::ostream& operator << (std::ostream& out,const glm::mat3& m)
{
	for (int i= 0; i < 3;++i)
	{
	for (int j=0; j<3;++j)
	{
		out << m[i][j]<<" ";
	}
	out<<"\n";
	}
	return out;
}

void GetTest(glm::mat3& test);

int main()
{
	glm::mat3 TestNormal = glm::mat3( 2,0,-4, 3, 5, 2, 7,3,0);
	glm::mat3 TestDiag = glm::mat3( 2,0,0, 0, 5, 0, 0,0,-6);
	glm::mat3 TestNullDeterminant = glm::mat3( -4,1,0, 3, 5, 0, 0,0,0);
	glm::mat3 TestCoplanarity = glm::mat3( 2,10,-40, 1, 5, -20, 7,3,2);
	glm::mat3 TestRandom = glm::mat3( 2,10,-40, 1, 5, -10, 7,3,2);
	glm::mat3 TestSmallNumbers = glm::mat3( 0.002,0.01,0, 0.1, 0.0005, -1.0, 0.07,0.3,0.02);
	glm::mat3 test = glm::mat3( 5.00 , 2.00 , 6.00,
  17.00 ,18.00 , 1.00,
   5.00,  7.00,  1.00);
	
	//---------------------------------
	std::cout << "Test normal matrixra: \nBemenet:\n";
	GetTest(TestNormal);
	
	std::cout << "Test daigonalis matrixra: \nBemenet:\n";
	GetTest(TestDiag);
	
	std::cout << "Test nulla determinansu matrixra: \nBemenet:\n";
	GetTest(TestNullDeterminant);
	
	std::cout << "Test koplanaris matrixra: \nBemenet:\n";
	GetTest(TestCoplanarity);
	
	std::cout << "Test koplanaris matrixra: \nBemenet:\n";
	GetTest(TestRandom);
	
/*	std::cout << "Test koplanaris matrixra: \nBemenet:\n";
	GetTest(TestSmallNumbers);*/
	
	std::cout << "Test koplanaris matrixra: \nBemenet:\n";
	GetTest(test);
	
	return 0;
}

void GetTest(glm::mat3& test)
{
	PlaneGetterFunctions<int>* OnlyForLU = new PlaneGetterFunctions<int>(std::vector<std::vector<int>>());
	std::cout<<test<<"\n";
	bool ok = true;
	PlaneGetterFunctions<int>::LuMatricies LU = OnlyForLU->GetLuDecomposition(&test,ok);
	std::cout<<"L es U matrixok: \n";
	std::cout<< LU << "\n";
	std::cout<<"Ellenorzes:\n";
	std::cout<<LU.U * LU.L * LU.P<<"\n";
	delete OnlyForLU;
}