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
	//TODO: Mikor rajzolom a v�g�s�kot a testhez a scale sk�l�zza a koordin�t�kat mind3 tengely ment�n �gy az eltol�st ennek megfelel�en
	//		x-1 el el kell tolni, ahol x a test sk�l�ja az egys�g kock�hoz k�pest    - ez nem okoz gondot ha �n mondok s�kot az enginek
	//		DE ott is figyelni kell erre !!!!!!!! (ford�tott eset)
	glm::mat4 GetTranslateFromCoord(glm::vec3, glm::vec3);
	glm::mat4 GetRotateFromNorm(glm::vec3);

	glm::vec3 DescartesToPolar(float, float );


};