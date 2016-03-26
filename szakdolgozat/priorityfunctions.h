#pragma once

#include "szakdoga\approximator.h"

template <class T>
class SorterFunctions
{
	/*TODO ide tenni egy pointert beginre,endre �s azon iter�lni v�gig + egy vectorpointer �s azt a sz�mot visszaadni*/
	//std::vector<int>* last_use;

	public:
		SorterFunctions() /*: last_use(NULL) */{ }
		~SorterFunctions() {}

	/*	void SetLastUse(std::vector<int>* lu)
		{
			last_use = lu;
		}*/
		float GetDiamaterLength(const T* atom) const
		{
			return atom->diameter().length();
		}

		float GetVolume(const T* atom) const
		{
			return atom->volume();
		}

		
};