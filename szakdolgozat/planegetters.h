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
	Utility::PlaneResult DiameterMerSulyp()
	 {
		 approx::Vector3<float> d = data->atoms(Active).diameter();
		 d.normalize();

		 approx::Vector3<float> norm;
		
		 if (d.z == 0)	//2D
		 {
			 norm = approx::Vector3<float>(-d.y, d.x, 0);
		 }
		 else
		 {
			 //RANDOMMAL a 2 fix pontot esetleg
			 float x = static_cast <float> (rd()) / static_cast <float> (rd.max());
			 float y = static_cast <float> (rd()) / static_cast <float> (rd.max());
			 norm = approx::Vector3<float>(1, 1, (-d.x * 1 - d.y * 1) / d.z) ;
		 }
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

};