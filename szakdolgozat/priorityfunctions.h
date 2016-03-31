#pragma once

#include "szakdoga\approximator.h"

template <class T>
class SorterFunctions
{
	std::vector<int>* last_use;
	/*approx::Approximation<float>::Iterator begin, end;*/

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
		
};

