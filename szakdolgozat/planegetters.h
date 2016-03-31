#pragma once

#include "Utils\Utility.h"

#include <random>

template <typename V>
class PlaneGetterFunctions
{
	V* data ;
	int Active;
	Utility::PlaneResult requestPlane;

	std::random_device rd;	//random generator

public:

	PlaneGetterFunctions(V* d = NULL, const int& a = 0) : data(d), Active(a) {}
	~PlaneGetterFunctions() {}

	void SetActive(const int& b)
		{
			Active = b;
		}
	void SetData(V* d)
	{
		data = d;		
	}

	void SetRequest(Utility::PlaneResult rp)
	{
		requestPlane = rp;
	}

	Utility::PlaneResult Manual()
	{
		return requestPlane;
	}
	Utility::PlaneResult Random()
	 {

		Utility::PlaneResult res(approx::Vector3<float>(rd(),rd(),rd()), approx::Vector3<float>(rd() % 100, rd() % 100, rd() % 100));

		return res;
	 }
	Utility::PlaneResult RandomNormalCentroid()
	 {
		 Utility::PlaneResult res(approx::Vector3<float>(rd(), rd(), rd()), data->atoms(Active).centroid() );

		return res;
	 }
	Utility::PlaneResult DiameterSulyp()
	 {
		 approx::Vector3<float> norm = data->atoms(Active).diameter();
		 norm.normalize();

		 Utility::PlaneResult res( norm, data->atoms(Active).centroid());

	   	 return res;
	 }
	Utility::PlaneResult RandomUnderFace()
	{
		std::vector<approx::Face<float>> faces = data->face_container();
		int randomFace = rd() % faces.size();

		approx::Plane<float> plane = faces[randomFace].to_plane();

		Utility::PlaneResult res(plane.normal(),plane.example_point());

		return res;
	}
	Utility::PlaneResult RandomSurface()
	{
		Utility::PlaneResult res(approx::Vector3<float>(rd(), rd(), rd()), approx::Vector3<float>(rd() % 100, rd() % 100, rd() % 100));

		return res;
	}
};

//ADJ. MTX
/*
Optimális felület:
 A ? nem csak [1..n]^2
 n = ? (háromszögek száma)
 m = ? (pontok száma)

 F(i,j+x) = i. háromszög (j+x) mod 3 . vertexe
 szimm diff F(k) = ???
 k = i*3 + j ?

 ez fordítva hogy N[i,j] = k ??

 F mutat egy pontra?
 és lényegében a pontok azonosíója kerül be az adott helyre

*************************************
Implementáció ötlet:

Szükség van:
 - pontokra + szomszédságukra VAGY GetterFv ami eldönti 2 pontról van e köztük él

- Saját típus, rá összeadás operátor
- F fv. megírása
- Szomszédság:
   - GetterFv szomszédságvizsgálatra , akár egybeépítve azzal is hogy visszaadjuk k-t, l-t
   - ez minden N mtx elemre
- A mtx felöltése

*/