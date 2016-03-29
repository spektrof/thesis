#pragma once

#include "szakdoga\approximator.h"

template <class T>
class SorterFunctions
{
	/*TODO ide tenni egy pointert beginre,endre �s azon iter�lni v�gig + egy vectorpointer �s azt a sz�mot visszaadni*/
	/*std::vector<int>* last_use;
	approx::Approximation<float>::Iterator begin, end;*/

	public:
		SorterFunctions() /*: last_use(NULL) */{ }
		~SorterFunctions() {}

		/*void SetLastUse(std::vector<int>* lu, approx::Approximation<float>::Iterator b, approx::Approximation<float>::Iterator e)
		{
			last_use = lu;
			begin = begin;
			end = e;
		}*/

		float GetDiamaterLength(const T* atom) const
		{
			return atom->diameter().length();
		}

		float GetVolume(const T* atom) const
		{
			return atom->volume();
		}

	/*	float GetLastUse(const T* atom) const
		{
			int counts = 0;
			for (approx::Approximation<float>::Iterator it = begin; it != end; ++it)
			{
				if (*it == atom)
				{
					return (*last_use)[counts];
				}
				counts++;
			}
		}*/
		
};