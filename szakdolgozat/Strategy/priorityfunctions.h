#pragma once

/*	Keszitette: Lukacs Peter
	
	A strategiahoz tartozo atomrendezes, ezen fuggvenyek alapjan hatarozza meg az atom adott tulajdonsagat.
*/

#include "..\Engine\approximator.h"

template <class T>
class SorterFunctions
{
	std::vector<int>* last_use;

public:
	SorterFunctions(std::vector<int>* l = NULL) : last_use(l) { }
	~SorterFunctions() {}

	/*Legregebben hasznalt vektor frissitese*/
	void SetLastUse(std::vector<int>* lu)
		{
			last_use = lu;
		}

	/*Atmero hosszat meghatoroz fv.*/
	float GetDiamaterLength(const T* atom,const int& _id) const
		{
			return atom->diameter().length();
		}

	/*Terfogatot meghatoroz fv.*/
	float GetVolume(const T* atom, const int& _id) const
		{
			return atom->volume();
		}

	/*Utoljara hasznalt idot hatarozza meg*/
	float GetLastUse(const T* atom, const int& _id) const
		{
			return (float)(*last_use)[_id];
		}
		
	/*Visszaadja az optimalis parametert a fourier egyutthatobol*/
	float GetOptimal(const T* atom, const int& _id) const
		{
			return GetOptimalFromFourier(atom->fourier());
		}

	/*Visszaadja az optimalis parameter es atmero kapcsolatabol megharozott erteket*/
	float GetOptimalAndDiameter(const T* atom, const int& _id) const
		{
			//return 10 * GetOptimalFromFourier(atom->fourier()) + atom->diameter().length();
			return GetOptimalFromFourier(atom->fourier()) * atom->diameter().length();
		}

	/*Visszaadja az optimalis parameter es terfogat kapcsolatabol megharozott erteket*/
	float GetOptimalAndVolume(const T* atom, const int& _id) const
		{
			//return 5000 * GetOptimalFromFourier(atom->fourier()) + atom->volume();
			return GetOptimalFromFourier(atom->fourier()) * atom->volume();
		}

protected:
	/*Optimalis parameter szamitas fourier egyutthatobol*/
	float GetOptimalFromFourier( float fourier, const float& a = 0.0f, const float& b = 0.0f) const
	{
		if (fourier > 0.5) fourier = 1 - fourier;

		return fourier < 0 ? 0 : 4 * (a + b - 4)* std::pow(fourier, 3) + 2 * (6 - 2 * a - b)* std::pow(fourier, 2) + a*fourier;
	}
};

