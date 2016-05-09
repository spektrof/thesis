#pragma once

/*	Keszitette: Lukacs Peter

A strategia altal meghatorozott sik szamitasa a vagashoz.
Minden sikvalaszto fuggveny itt talalhato.
*/

#include <random>
#define FACEEPSILON 0.0001f
#define DISTANCEEPSILON  2.0f

template <typename V>
class PlaneGetterFunctions
{
	V* data;
	std::vector< std::vector<int>> adj_mtx;
	int Active;
	Utility::PlaneResult requestPlane;

	std::random_device rd;	//random generator
#ifdef ONLYFORTEST
	public:
#endif
	struct LuMatrices
	{
		glm::mat3 L;
		glm::mat3 U;
		glm::mat3 P;
		LuMatrices(const glm::mat3& l = glm::mat3(1.0f), const glm::mat3& u = glm::mat3(1.0f), const glm::mat3& p = glm::mat3(1.0f)) : L(l), U(u), P(p) {}
	};

	Utility::PlaneResult PlanarFittingOn3dPoints(const std::set<approx::Vector3<float>, approx::DifferentVector3<float>>&, bool&, bool&, const float& = INFINITY);

	LuMatrices GetLuDecomposition(glm::mat3*, bool& copl);
	glm::vec3 GetSolutionOfLER(const LuMatrices*,  glm::vec3&);
	glm::mat3 GetPivot(glm::mat3& A);
	float GetDistanceValue(const glm::vec3&,const std::vector<approx::Vector3<float>>&);
	int GetColumnNormMinOf3x3Matrix(const glm::mat4x3&);
	int GetColumnNormMinOf3x3Matrix(const glm::mat3&);
	float GetDeterminantOf3x3Matrix(const glm::mat3&);

public:

	PlaneGetterFunctions(std::vector< std::vector<int>> adj, V* d = NULL, const int& a = 0, const Utility::PlaneResult req = Utility::PlaneResult())
		: data(d), adj_mtx(adj), Active(a), requestPlane(req) {}
	~PlaneGetterFunctions() {}

	void SetActive(const int& b)
	{
		Active = b;
	}
	void SetData(V* d)
	{
		data = d;
	}
	void SetAdjMatrix(std::vector< std::vector<int>> a)
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
		Utility::PlaneResult res(approx::Vector3<float>((rd() % 11) - 5.0f, (rd() % 11) - 5.0f, (rd() % 11) - 5.0f), data->atoms(Active).centroid());
		res.normal.normalize();
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
		std::vector<approx::Face<float>> faces = data->atoms(Active).safe_cutting_faces_inside(FACEEPSILON);

		if (faces.size() == 0) return Utility::PlaneResult();

		int randomFace = rd() % faces.size();

		approx::Plane<float> plane = faces[randomFace].to_plane();
		approx::Vector3<float> norm = plane.normal();
		Utility::PlaneResult res(plane.normal(), plane.example_point() - norm *FACEEPSILON);
		return res;
	}

	Utility::PlaneResult RandomSurface()
	{
		/*Lekerem az beeseo lapok indexeit, ha azok nem voltak akkor egy default sikkal terek vissza*/
		std::vector<int> tmp = data->atoms(Active).face_indicies_inside();
		if (tmp.size() == 0) return Utility::PlaneResult();

		/*Ha volt beeso lap akkor csinalok egy maximum indexu lap hosszusagu vektort keszitek, majd feloltoltem ott 1-sekkel amelyik sik benne van*/
		std::vector<approx::Face<float>> faces = data->target_body().face_container();

		std::vector<int> ids;
		ids.resize(*(std::max_element(tmp.begin(), tmp.end())) + 1);
		for (std::vector<int>::iterator it = tmp.begin(); it != tmp.end(); ++it) { ids[*it] = 1; }

		/*Random oldal kiválasztása */
		int randomFace = rd() % tmp.size();

		/*Keszitek egy used vektort arra, hogy megtudjam a beeso lapok kozul melyiket talaltam meg a szomszedsagi terjesztes soran
		es egy halmazt a terjeszteshez	*/
		std::vector<int> used;
		std::set<int> active;
		std::set<int> iter;

		active.clear();
		active.insert(tmp[randomFace]);
		used.resize(adj_mtx.size());
		used[tmp[randomFace]] = 1;

		std::set< approx::Vector3<float>, approx::DifferentVector3<float>> points;
		Utility::PlaneResult res;
		bool ok = true;

		/*Oldal kiterjesztese szomszedsagi matrix alapjan*/
		while (active.size() > 0)
		{
			int index = *(active.begin());
			active.erase(active.begin());

			for (int i = 0; i < 3; ++i)	//feltetelezzuk, hogy haromszogelve van minden oldal
			{
				const int adj = adj_mtx[index][i];
				if (!used[adj] && adj < ids.size() && ids[adj])	// used: HA 0 -> igaz (nem volt még)
				{
					iter.insert(adj);
					used[adj] = 1;
				}
			}
			/*Berakom a pontokat*/
			for (int j = 0; j < 3; ++j)
			{
				points.insert(faces[index].points(j));
			}

			if (active.size() == 0)	//akkor illesztunk pontot mikor az adott iteracio lepesben minden oldalt megneztunk (eleres a kezdo oldaltol)
			{
				active = iter;
				iter.clear();
				bool coplanarity = false;
				Utility::PlaneResult tmp_res = PlanarFittingOn3dPoints(points, ok, coplanarity, DISTANCEEPSILON);	
																								
				if (ok && !coplanarity )
				{
					res = tmp_res;
				}
				else if (!ok)
					break;
			}
		}
		return res;
	}
	
	Utility::PlaneResult AllPointsFitting()
	{
		/*Lekerem az osszes beeso lapot, ha nem volt akkor default sikkal terek vissza*/
		std::vector<approx::Face<float>> faces = data->atoms(Active).faces_inside();
		if (faces.size() == 0) return Utility::PlaneResult();

		/*Ha volt akkor osszeszedem az osses lap pontjait ismetlodes nelkul*/
		std::set<approx::Vector3<float>, approx::DifferentVector3<float>> vertexes;
		vertexes.clear();

		for (std::vector<approx::Face<float>>::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			for (approx::Face<float>::VertexIterator vit = it->begin(); vit != it->end(); ++vit)
			{
				vertexes.insert(*vit);
			}
		}
		/*Illesztek rajuk sikot*/
		bool ok = true, cop = false;
		Utility::PlaneResult res = PlanarFittingOn3dPoints(vertexes,ok,cop);

		return res;
	}

};

/*LU decompozicio , A = P * L * U */
template <typename V>
typename PlaneGetterFunctions<V>::LuMatrices PlaneGetterFunctions<V>::GetLuDecomposition(glm::mat3* A,bool& copl)
{
	//szingularis-e
	if (GetDeterminantOf3x3Matrix(*A) == 0) {
		copl = true;
		return LuMatrices();
	}

	glm::mat3 P = GetPivot(*A);
	*A = (*A) * glm::inverse(P); 

	glm::mat3 L = glm::mat3(1.0f);
	glm::mat3 U = *A;
	/* Note: glm::mat3 szorzás : A * B az valójában B * A */
	for (int i = 0; i < 3;++i)
	{		
		glm::mat3 Lk = glm::mat3(1.0f);
		glm::mat3 LIk = glm::mat3(1.0f);

		for (int j = i + 1; j < 3; ++j)
		{
			Lk[j][i] = -U[j][i] / U[i][i];
			LIk[j][i] = U[j][i] / U[i][i];
		}

		L = LIk * L;
		U *= Lk;
	}

	return LuMatrices(L, U, P);
}

/* A = LU után & b vektor segítségével megoldja a LERT*/
template <typename V>
glm::vec3 PlaneGetterFunctions<V>::GetSolutionOfLER(const LuMatrices* LU,  glm::vec3& b)
{
	b = b * glm::inverse(LU->P);	// A * x = P * L * U * x = n -> L * U * x = inv(P) * x

	glm::vec3 x, y;

	/* L * y = b */
	for (int i = 0; i < 3; ++i)
	{
		float tmp = 0;
		for (int j = 0; j < i;++j)
		{
			tmp += y[j] * LU->L[i][j];
		}
		y[i] = (b[i] - tmp) / LU->L[i][i];
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

/*Sikillesztes
(pontok halmaza, bool - hataron beluli eredmenyt kaptunk, bool - koplanarisok voltak e a pontok, hatar)*/
template <typename V>
Utility::PlaneResult PlaneGetterFunctions<V>::PlanarFittingOn3dPoints(const std::set<approx::Vector3<float>, approx::DifferentVector3<float>>& tmp, bool& ok, bool& coplanarity, const float& DISTANCE)
{
	std::vector<approx::Vector3<float>> points = std::vector<approx::Vector3<float>>(tmp.begin(), tmp.end());	
	//cpp ref: neither the const nor the non-const versions modify the container

	/*First step: sulypont meghatarozasa */
	approx::Vector3<float> centr = approx::Vector3<float>(0.0f, 0.0f, 0.0f);

	for (std::vector<approx::Vector3<float>>::iterator it = points.begin(); it != points.end(); ++it)
	{
		centr += *it;
	}
	/*sulypontok*/
	centr /= (float)points.size();

	/*Second step: atlag levonasa a pontokbol*/
	for (std::vector<approx::Vector3<float>>::iterator it = points.begin(); it != points.end(); ++it)
	{
		*it -= centr;
	}

	/*Third Step: LER keszitese:*/
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
	}

	int comp = GetColumnNormMinOf3x3Matrix(LER_left);
	LER_left[3][comp] = 1;

	/*Fourth step: LU felbontas*/
	/*tulhatarozott -> Gauss fele normalegyenlet A^T * A * x = A^T * b */
	glm::mat3 A = LER_left * glm::transpose(LER_left);
	glm::vec3 b = LER_right * glm::transpose(LER_left);

	LuMatrices result = GetLuDecomposition(&A, coplanarity);
	if (coplanarity)
	{
#ifdef ONLYFORTEST
		std::cout << "Coplanarity - LU\n";
#endif
		return Utility::PlaneResult();
	}
	
	/*Fifth step: Get our x = (A , B , C)*/
	glm::vec3 x = GetSolutionOfLER(&result, b);
	glm::vec3 check = x * A;

	if (true == glm::any(glm::isnan(x))) {
#ifdef ONLYFORTEST
		std::cout << "Coplanarity\n";
#endif
		coplanarity = true;
		return Utility::PlaneResult();
	}

	/*Sixth step: Get our normal*/
	glm::vec3 normal = glm::normalize(glm::vec3(x.x, x.y, x.z));

	ok = GetDistanceValue(normal,points) <  DISTANCE;
	if (!ok) return Utility::PlaneResult();

	return Utility::PlaneResult(approx::convert<float>(normal), centr + approx::convert<float>(normal * FACEEPSILON));
}

/*A pontoknak a sik normalisaval vett skaliri szorzatok osszege*/
template <typename V>
float PlaneGetterFunctions<V>::GetDistanceValue(const glm::vec3& v, const std::vector<approx::Vector3<float>>& po)
{
	float sum=0;
	for (std::vector<approx::Vector3<float>>::const_iterator it = po.begin(); it != po.end(); ++it)
	{
		sum += std::pow(glm::dot(approx::convert<float>(*it), v),2);
	}
	return sum;
}

/*Visszaadja a permutacios matrixot*/
template <typename V>
glm::mat3 PlaneGetterFunctions<V>::GetPivot(glm::mat3& A)
{
	glm::mat3 pivot = glm::mat3(1.0f);
	
	for (int i=0; i<2; ++i)
	{
		float max = A[i][i];
		int row = i;
		for (int j=i;j<3;++j)
		{
			if (A[j][i] > max)
			{
				max = A[j][i];
				row = j;
			}
		}
		if (i != row)
		{
			for (int j=0;j<3;++j)
			{
				float tmp = pivot[i][j];
				pivot[i][j] = pivot[row][j];
				pivot[row][j] = tmp;
			}
		}
	}
	return pivot;
}

/*3x3-as bal felso reszmatrixon megkeresi melyik oszlopban a legkisebb a szamok abszolutertekeinek az osszege*/
template <typename V>
int PlaneGetterFunctions<V>::GetColumnNormMinOf3x3Matrix(const glm::mat4x3& m)
{
	int res = 0;
	float minval = 0;
	for (int i = 0;i < 3;++i)
	{
		minval += std::abs(m[i][0]);
	}

	for (int i = 1; i < 3;++i)
	{
		float tmp = 0;
		for (int j = 0; j < 3; ++j)
		{
			tmp += std::abs(m[j][i]);
		}
		if (tmp < minval)
		{
			minval = tmp;
			res = i;
		}
	}
	return res;
}

/*Megkeresi melyik oszlopban a legkisebb a szamok abszolutertekeinek az osszege*/
template <typename V>
int PlaneGetterFunctions<V>::GetColumnNormMinOf3x3Matrix(const glm::mat3& m)
{
	int res = 0;
	float minval = 0;
	for (int i = 0;i < 3;++i)
	{
		minval += std::abs(m[i][0]);
	}

	for (int i = 1; i < 3;++i)
	{
		float tmp = 0;
		for (int j = 0; j < 3; ++j)
		{
			tmp += std::abs(m[j][i]);
		}
		if (tmp < minval)
		{
			minval = tmp;
			res = i;
		}
	}
	return res;
}

/*3x3-as matrix determinansa, Sarrus szabaly*/
template <typename V>
float PlaneGetterFunctions<V>::GetDeterminantOf3x3Matrix(const glm::mat3& matrix_3x3)
{
	float sum_one = 0, sum_two = 0;
	for (int i = 0; i < 3; ++i)
	{
		sum_one += matrix_3x3[0][(0 + i) % 3] * matrix_3x3[1][(1 + i) % 3] * matrix_3x3[2][(2 + i) % 3];
		sum_two += matrix_3x3[2][(0 + i) % 3] * matrix_3x3[1][(1 + i) % 3] * matrix_3x3[0][(2 + i) % 3];
	}
		
	 return sum_one - sum_two;
}