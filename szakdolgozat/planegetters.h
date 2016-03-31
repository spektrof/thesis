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
Optim�lis fel�let:
 A ? nem csak [1..n]^2
 n = ? (h�romsz�gek sz�ma)
 m = ? (pontok sz�ma)

 F(i,j+x) = i. h�romsz�g (j+x) mod 3 . vertexe
 szimm diff F(k) = ???
 k = i*3 + j ?

 ez ford�tva hogy N[i,j] = k ??

 F mutat egy pontra?
 �s l�nyeg�ben a pontok azonos��ja ker�l be az adott helyre

*************************************
Implement�ci� �tlet:

Sz�ks�g van:
 - pontokra + szomsz�ds�gukra VAGY GetterFv ami eld�nti 2 pontr�l van e k�zt�k �l

- Saj�t t�pus, r� �sszead�s oper�tor
- F fv. meg�r�sa
- Szomsz�ds�g:
   - GetterFv szomsz�ds�gvizsg�latra , ak�r egybe�p�tve azzal is hogy visszaadjuk k-t, l-t
   - ez minden N mtx elemre
- A mtx fel�lt�se

*/