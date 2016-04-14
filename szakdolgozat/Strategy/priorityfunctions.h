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
	SorterFunctions() : last_use(NULL) { }
	~SorterFunctions() {}

	void SetLastUse(std::vector<int>* lu)
		{
			last_use = lu;
		}

	float GetDiamaterLength(const T* atom,const int& _id) const
		{
			return atom->diameter().length();
		}

	float GetVolume(const T* atom, const int& _id) const
		{
			return atom->volume();
		}

	float GetLastUse(const T* atom, const int& _id) const
		{
			return (float)(*last_use)[_id];
		}
		
	float GetOptimal(const T* atom, const int& _id) const
		{
			return GetOptimalFromFourier(atom->fourier());
		}

	float GetOptimalAndDiameter(const T* atom, const int& _id) const
		{
			//return 10 * GetOptimalFromFourier(atom->fourier()) + atom->diameter().length();
			return GetOptimalFromFourier(atom->fourier()) * atom->diameter().length();
		}

	float GetOptimalAndVolume(const T* atom, const int& _id) const
		{
			//return 5000 * GetOptimalFromFourier(atom->fourier()) + atom->volume();
			return GetOptimalFromFourier(atom->fourier()) * atom->volume();
		}

protected:
	float GetOptimalFromFourier( float fourier, const float& a = 0.0f, const float& b = 0.0f) const
	{
		if (fourier > 0.5) fourier = 1 - fourier;

		return fourier < 0 ? 0 : 4 * (a + b - 4)* std::pow(fourier, 3) + 2 * (6 - 2 * a - b)* std::pow(fourier, 2) + a*fourier;
	}
};

