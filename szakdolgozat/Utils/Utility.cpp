#include "Utility.h"

glm::vec3 Utility::DescartesToPolar(const float& omega,const float& theta,const float& r)
{
	return glm::vec3(r * cosf(omega)*sinf(theta),r * cosf(theta),r * sinf(omega)*sinf(theta));
}

glm::mat4 Utility::GetTranslate(const approx::Vector3<float>& centroid, const glm::vec3& normal, const float& distance)
{
	return glm::translate<float>(	centroid.x - (normal.x * distance),
									centroid.y - (normal.y * distance),
									centroid.z - (normal.z * distance));
}
/*Visszaadja a forgatasi matrixot
  Bemenete: normalizalt vektor!*/
glm::mat4 Utility::GetRotateFromNorm(const glm::vec3& vec)
{
	if (vec == glm::vec3(0,0,0)) return glm::rotate<float>(0, 1.0f, 0.0f, 0.0f); 
	
	glm::vec3 axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), vec);	//tengely

	float angle = glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), vec));	//szog
	
	#ifdef ONLYFORTEST
		std::cout<<"Forgatasi szog: "<< angle * 180.0f / (float)M_PI<<"\n";
	#endif
	
	if (angle == 0.0f)
		return glm::rotate<float>(angle, 1.0f, 0.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizalni - helybenhagyas
	if (angle == (float)M_PI)
		return glm::rotate<float>(angle * 180.0f / (float)M_PI, 0.0f, 1.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - fordulas

	axis = glm::normalize(axis);
	
	#ifdef ONLYFORTEST
		std::cout<<"Forgatasi tengely: "<< axis <<"\n";
	#endif
	
	return glm::rotate<float>(angle * 180.0f / (float)M_PI, axis.x, axis.y, axis.z);
}

std::vector< std::vector<int>> Utility::GetAdjacencyMatrix(const std::vector<approx::Face<float>>* faces)
{
	if (faces->size() == 0) return std::vector< std::vector<int>>();

	const int faceSize = (int)faces->size();
	const size_t vertexSize = (*faces)[0].vertex_container()->size();
	std::vector< std::vector<int>> adj_mtx, FN;

	/*Resize*/
	FN.resize(vertexSize);
	for (size_t i = 0; i < vertexSize; ++i)
	{
		FN[i].resize(vertexSize);
	}

	adj_mtx.resize(faceSize);
	for (size_t i = 0; i < faceSize; ++i)
	{
		adj_mtx[i].resize(3);
	}
	/*-----------------------------------------------------*/
	/*vertex * vertex meretu matrix feltoltese*/
	for (int l = 0; l < faceSize; ++l)
	{
		std::vector<int> ind = (*faces)[l].indicies();
		const int i = ind[0], j = ind[1], k = ind[2];
		if (FN[i][j] == 0) FN[i][j] = l; else FN[j][i] = l;
		if (FN[j][k] == 0) FN[j][k] = l; else FN[k][j] = l;
		if (FN[k][i] == 0) FN[k][i] = l; else FN[i][k] = l;
	}

	/*---------------------------------------------------*/
	/*Oldalszomszedsagi matrix feltoltese*/
	for (size_t l = 0; l < faceSize; ++l)
	{
		std::vector<int> ind = (*faces)[l].indicies();
		const int i = ind[0], j = ind[1], k = ind[2];

		adj_mtx[l][0] = FN[i][j] == l ? FN[j][i] : FN[i][j];
		adj_mtx[l][1] = FN[j][k] == l ? FN[k][j] : FN[j][k];
		adj_mtx[l][2] = FN[k][i] == l ? FN[i][k] : FN[k][i];
	}

	return adj_mtx;
}