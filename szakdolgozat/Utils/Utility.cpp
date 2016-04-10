#include "Utility.h"

glm::vec3 Utility::DescartesToPolar(float omega, float theta)
{
	return glm::vec3(cosf(omega)*sinf(theta), cosf(theta), sinf(omega)*sinf(theta));
}

glm::mat4 Utility::GetTranslate(const approx::Vector3<float>& centroid, const glm::vec3& normal, const float& distance)
{
	return glm::translate<float>(	centroid.x - (normal.x * distance),
									centroid.y - (normal.y * distance),
									centroid.z - (normal.z * distance));
}
glm::mat4 Utility::GetRotateFromNorm(const glm::vec3& vec)
{
	glm::vec3 axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), vec);

	float angle = glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), vec));
	if (angle == 0.0f)
		return glm::rotate<float>(angle, 1.0f, 0.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - helybenhagyás
	if (angle == (float)M_PI)
		return glm::rotate<float>(angle * 180.0f / (float)M_PI, 0.0f, 1.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - fordulás

	axis = glm::normalize(axis);
	return glm::rotate<float>(angle * 180.0f / (float)M_PI, axis.x, axis.y, axis.z);
}

/*-----------------------------*/

std::vector< std::vector<int>> Utility::GetAdjacencyMatrix2(const std::vector<approx::Face<float>>* faces)
{
	std::vector< std::vector<int>> adj_mtx;

	/*Resize*/
	adj_mtx.resize(faces->size());
	for (size_t i = 0; i < adj_mtx.size(); ++i)
	{
		adj_mtx[i].resize(3);
	}

	/*Set*/
	std::set<int> usedFaces;
	std::set<int> recentFaces;

	recentFaces.insert(0);
	/*Fill the matrix*/
	while (recentFaces.size() > 0)
	{
		int face_id = *(recentFaces.begin());
		
		for (int i = 0; i < 3; ++i)
		{
			if (adj_mtx[face_id][i] != 0) continue;

			int adj = 0;
			int edge = 0;
									/* Oldalak , akt. oldal, szoméd id, használt oldalak, melyik el (faces_id), melyik el (adj)*/
			bool IsAdj = GetAdjacency(faces, face_id, adj, &usedFaces, i, edge);

			if (IsAdj)	//safety: sikeresen megtaláltuk a másik oldalt
			{
				adj_mtx[face_id][i] = adj + 1;	//mert 0 azt jelenti hogy nincs szomszedja -> 0. face is lehet szomszed
				adj_mtx[adj][edge] = face_id + 1;
				recentFaces.insert(adj);	//halmaz, nem problem - ha pedig már használva lett volna nem lenne 0 ez az elem az A-ban
			}
		}

		usedFaces.insert(face_id);
		recentFaces.erase(face_id);
	}

	/*Visszaredukálás (+1 -ek) ; Trick: 1-es indexel kezdenénk és ha 0 a szomszéd akkor azzal nem foglalkozunk -> a végén a 3 inicializáló 0 marad*/
	for (int i = 0; i < faces->size();++i)
	{
		for (int j = 0; j < 3;++j)
		{
			adj_mtx[i][j]--;
		}
	}

	return adj_mtx;
}
/*Megkeresi a face_id oldalnak az i.oldalához tartozó másik oldalt!
  Megj.: targetnál ccw ben vannak a pontok ( . points(i) -ben )*/
bool Utility::GetAdjacency(const std::vector<approx::Face<float>>* faces, const int& face_id, int& adj, const std::set<int>* used, const int& i, int& edge)
{
	approx::Vector3<float> point_1 = (*faces)[face_id].points( i );
	approx::Vector3<float> point_2 = (*faces)[face_id].points( (i+1) % 3 );

	for (size_t it = 0; it < faces->size(); ++it)
	{
		if (/*used->find((int)it) != used->end() ||*/ (int)it == face_id) continue;	//már használtuk || épp vele dolgozunk

		for (int vertex = 0; vertex < 3; ++vertex)
		{
			approx::Vector3<float> point_3 = (*faces)[it].points(vertex);
			approx::Vector3<float> point_4 = (*faces)[it].points((vertex + 1) % 3);

			if ( point_3 == point_1 && point_4 == point_2)	//ez elv le sem futhat! ccw miatt
			{
				edge = vertex;
				adj = (int)it;
				return true;
			}
			if (point_4 == point_1 && point_3 == point_2)
			{
				edge = vertex ;
				adj = (int)it;
				return true;
			}
		}
	}

	return false;
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

	for (int l = 0; l < faceSize; ++l)
	{
		std::vector<int> ind = (*faces)[l].indicies();
		const int i = ind[0], j = ind[1], k = ind[2];
		if (FN[i][j] == 0) FN[i][j] = l; else FN[j][i] = l;
		if (FN[j][k] == 0) FN[j][k] = l; else FN[k][j] = l;
		if (FN[k][i] == 0) FN[k][i] = l; else FN[i][k] = l;
	}

	/*---------------------------------------------------*/
	
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