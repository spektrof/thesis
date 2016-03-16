#pragma once

#include <SDL.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <vector> // pair miatt kell

namespace Utility
{
	typedef std::pair<int, float> data_t;

	template<class T>
	struct greater_second
		: std::binary_function<T, T, bool>
	{
		inline bool operator()(const T& lhs, const T& rhs)
		{
			return lhs.second > rhs.second;
		}
	};
	//TODO: Mikor rajzolom a vágósíkot a testhez a scale skálázza a koordinátákat mind3 tengely mentén így az eltolást ennek megfelelõen
	//		x-1 el el kell tolni, ahol x a test skálája az egység kockához képest    - ez nem okoz gondot ha én mondok síkot az enginek
	//		DE ott is figyelni kell erre !!!!!!!! (fordított eset)
	glm::mat4 GetTranslateFromCoord(glm::vec3, glm::vec3);
	glm::mat4 GetRotateFromNorm(glm::vec3);

	glm::vec3 DescartesToPolar(float, float );


};