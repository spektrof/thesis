#pragma once

/*	Keszitette: Lukacs Peter

	A strategia altal meghatorozott sik szamitasa a vagashoz.
	Minden sikvalaszto fuggveny itt talalhato.
*/

#include <random>
#define FACEEPSILON 0.0001f

template <typename V>
class PlaneGetterFunctions
{
	V* data;
	std::vector< std::vector<int>>* adj_mtx;
	int Active;
	Utility::PlaneResult requestPlane;

	std::random_device rd;	//random generator

	struct LuMatricies
	{
		glm::mat3 L;
		glm::mat3 U;
		LuMatricies(const glm::mat3& l, const glm::mat3& u) : L(l), U(u) {}
	};

	Utility::PlaneResult PlanarFittingOn3dPoints(const std::set<approx::Vector3<float>, approx::DifferentVector3<float>>&);
	LuMatricies GetLuDecomposition(glm::mat3*);
	glm::vec3 GetSolutionOfLER(const LuMatricies*, const glm::vec3*);

public:

	PlaneGetterFunctions(V* d = NULL, std::vector< std::vector<int>>* adj = NULL, const int& a = 0, const Utility::PlaneResult req = Utility::PlaneResult() ) : data(d), adj_mtx(adj), Active(a),requestPlane(req) {}
	~PlaneGetterFunctions() {}

	void SetActive(const int& b)
	{
		Active = b;
	}
	void SetData(V* d)
	{
		data = d;
	}
	void SetAdjMatrix(std::vector< std::vector<int>>* a)
	{
		adj_mtx = a;
	}
	void SetRequest(Utility::PlaneResult rp)
	{
		requestPlane = rp;
	}

	/*Vagasi strategiakhoz a sikszamito fuggvenyek*/
	Utility::PlaneResult Manual()
	{
		return requestPlane;
	}

	Utility::PlaneResult RandomNormalCentroid()
	{
		Utility::PlaneResult res(approx::Vector3<float>((float)rd(), (float)rd(), (float)rd()), data->atoms(Active).centroid());

		return res;
	}

	Utility::PlaneResult DiameterCentroid()
	{
		approx::Vector3<float> norm = data->atoms(Active).diameter();
		norm.normalize();

		Utility::PlaneResult res(norm, data->atoms(Active).centroid());

		return res;
	}

	Utility::PlaneResult RandomUnderFace()
	{
		std::vector<approx::Face<float>> faces = data->atoms(Active).faces_inside();

		if (faces.size() == 0) return Utility::PlaneResult(approx::Vector3<float>(1, 0, 0), approx::Vector3<float>(0, 0, 0));

		int randomFace = rd() % faces.size();

		approx::Plane<float> plane = faces[randomFace].to_plane();
		approx::Vector3<float> norm = plane.normal();
		Utility::PlaneResult res(plane.normal(), plane.example_point() + approx::Vector3<float>(norm.x * FACEEPSILON, norm.y * FACEEPSILON, norm.z * FACEEPSILON));
		return res;
	}

	Utility::PlaneResult RandomSurface()
	{
		std::vector<int> tmp = data->atoms(Active).face_indicies_inside();
		if (tmp.size() == 0) return Utility::PlaneResult(approx::Vector3<float>(1, 0, 0), approx::Vector3<float>(0, 0, 0));

		std::vector<approx::Face<float>> faces = data->target_body().face_container();

		std::vector<int> ids;
		ids.resize(*(std::max_element(tmp.begin(),tmp.end()))+1);
		for (std::vector<int>::iterator it = tmp.begin(); it != tmp.end(); ++it) { ids[*it] = 1; }

		/*Random oldal kiválasztása + szomszédsági mátrix -> pontok halmaza -> pontokra síkillesztés*/
		int randomFace = rd() % tmp.size();

		std::vector<int> used;
		std::set<int> active;

		active.clear();
		active.insert(tmp[randomFace]);
		used.resize(adj_mtx->size());
		used[tmp[randomFace]] = 1;

		/*Faces stretch*/
		while (active.size() > 0)
		{
			int index = *(active.begin());
			active.erase(active.begin());

			for (int i = 0;i < 3;++i)
			{
				const int adj = (*adj_mtx)[index][i];
				if ( !used[adj] && adj < ids.size() && ids[adj] )	// HA 0 -> igaz (nem volt még)
				{
					active.insert(adj);
					used[adj] = 1;
				}
			}
		}

		/*Get our different points*/
		std::set< approx::Vector3<float>, approx::DifferentVector3<float>> points;

		for (size_t i = 0; i < tmp.size();++i)
		{
			if (!used[tmp[i]]) continue;	// Nem tagja a felületnek

			for (int j = 0; j < 3; ++j)
			{
				points.insert(faces[i].points(j));
			}
		}

		Utility::PlaneResult res = PlanarFittingOn3dPoints(points);

		return res;
	}

	Utility::PlaneResult AllPointsFitting()	
	{
		std::vector<approx::Face<float>> faces = data->atoms(Active).faces_inside();
		if (faces.size() == 0) return Utility::PlaneResult(approx::Vector3<float>(1, 0, 0), approx::Vector3<float>(0, 0, 0));

		std::set<approx::Vector3<float>, approx::DifferentVector3<float>> vertexes;
		vertexes.clear();

		for (size_t i = 0; i < faces.size();++i)
		{
			for (size_t j = 0; j < faces[i].size(); ++j)
			{
				vertexes.insert(faces[i].points(j));
			}
		}

		Utility::PlaneResult res = PlanarFittingOn3dPoints(vertexes);

		return res;
	}
};


/* (x,y,f(x,y)) síkillesztés 3D pontokra
Bemente: a 3D pontok vektora
Kimenete: a sík normálisa + egy pontja  */
template <typename V>
Utility::PlaneResult PlaneGetterFunctions<V>::PlanarFittingOn3dPoints(const std::set<approx::Vector3<float>, approx::DifferentVector3<float>>& tmp)	/*set mert legyen minden pont kül.*/
{
	std::vector<approx::Vector3<float>> points = std::vector<approx::Vector3<float>>(tmp.begin(), tmp.end());
	//vector mert a setnek csak const iteratora van -> ill cond nem tudom megcsinálni

	/*First step: averages - cause ill-conditioned*/
	float _x = 0, _y = 0, _z = 0;

	for (std::vector<approx::Vector3<float>>::iterator it = points.begin(); it != points.end(); ++it)
	{
		_x += it->x;
		_y += it->y;
		_z += it->z;
	}
	/*sulypontok*/
	_x /= points.size();
	_y /= points.size();
	_z /= points.size();

	/*Second step: difference - original & old*/

	for (std::vector<approx::Vector3<float>>::iterator it = points.begin(); it != points.end(); ++it)
	{
		it->x -= _x;
		it->y -= _y;
		it->z -= _z;
	}

	/*Third Step: LER creating*/
	
	glm::mat4x3 LER_left = glm::mat4x3(0.0f);
	glm::vec4 LER_right = glm::vec4(0, 0, 0, 1);

	for (std::vector<approx::Vector3<float>>::iterator it = points.begin(); it != points.end(); ++it)
	{
		LER_left[0][0] += pow(it->x, 2);
		LER_left[0][1] = LER_left[1][0] += it->x * it->y;
		LER_left[0][2] = LER_left[2][0] += it->x * it->z;
		LER_left[1][1] += pow(it->y, 2);
		LER_left[1][2] = LER_left[2][1] += it->y * it->z;
		LER_left[2][2] += pow(it->z, 2);
	//	LER_right[0] += std::abs(it->x * it->z);
		//LER_right[1] += std::abs(it->y * it->z);
	//	LER_right[2] += std::abs(it->z);
	}
	/*LER_left[0][0] = 2;
	LER_left[0][1] = 3;
	LER_left[0][2] = 0;
	LER_left[1][0] = 1;
	LER_left[1][1] = 2;
	LER_left[1][2] = 15;
	LER_left[2][0] = 23;
	LER_left[2][1] = 1;
	LER_left[2][2] = 67;*/

	LER_left[3][0] = LER_left[3][1] = 0 ;
	LER_left[3][2] = 1;
	/*Fourth step: LU decomposition*/

	/*tulhatarozott -> Gauss fele normalegyenlet A^T * A * x = A^T * y */
	glm::mat3 A = LER_left * glm::transpose(LER_left);
	glm::vec3 y = LER_right * glm::transpose(LER_left);

	LuMatricies result = GetLuDecomposition(&A);

	/*Fifth step: Get our x = (A , B , C)*/
	glm::vec3 x = GetSolutionOfLER(&result, &y);
	glm::vec3 check = x * A;

	/*Sixth step: Get our normal*/

	glm::vec3 normal = glm::normalize(glm::vec3(x.x, x.y, x.z));

	return Utility::PlaneResult(approx::Vector3<float>(normal.x, normal.y, normal.z), approx::Vector3<float>(_x, _y, _z));
}

/*LU decompozíció
Bemenete: A felbontani kívánt mátrix
Kiemnete: L és U mátrixok	*/
template <typename V>
typename PlaneGetterFunctions<V>::LuMatricies PlaneGetterFunctions<V>::GetLuDecomposition(glm::mat3* A)
{
	glm::mat3 L = glm::mat3(1.0f);
	glm::mat3 U = *A;
	/* Note: glm::mat3 szorzás : A * B az valójában B * A */
	for (int i = 0; i < 3;++i)
	{
		glm::mat3 Lk = glm::mat3(1.0f);
		glm::mat3 LIk = glm::mat3(1.0f);

		for (int j = i + 1; j < 3; ++j)
		{
			Lk[j][i] = -(*A)[j][i] / (*A)[i][i];
			LIk[j][i] = (*A)[j][i] / (*A)[i][i];
		}

		L = LIk * L;
		U *= Lk;
		(*A) = (*A) * Lk;
	}

	glm::mat3 check = U * L;

	return LuMatricies(L, U);
}

/* A = LU után & b vektor segítségével megoldja a LERT*/
template <typename V>
glm::vec3 PlaneGetterFunctions<V>::GetSolutionOfLER(const LuMatricies* LU, const glm::vec3* b)
{
	glm::vec3 x,y;

	/* L * y = b */
	for (int i = 0; i < 3; ++i)
	{
		float tmp = 0;
		for (int j = 0; j < i;++j)
		{
			tmp += y[j] * LU->L[i][j];
		}
		y[i] = ((*b)[i] - tmp) / LU->L[i][i];
	}

	/* U * x = y */
	for (int i = 2; i >= 0; --i)
	{
		float tmp = 0;
		for (int j = 2; j > i;--j)
		{
			tmp += x[j] * LU->U[i][j];
		}
		x[i] = (y[i] - tmp) / LU->U[i][i];
	}

	return x;
}