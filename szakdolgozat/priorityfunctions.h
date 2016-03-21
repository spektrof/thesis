#pragma once

#include "szakdoga\approximator.h"

template <class T>
class SorterFunctions
{
	public:
		float GetDiamaterLength(T atom) const
		{
			return atom.diameter().length();
		}

		float GetVolume(T atom) const
		{
			return atom.volume();
		}
};