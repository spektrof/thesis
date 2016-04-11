#pragma once

#include <SDL.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <vector> // pair miatt kell
#include <iostream> // pair miatt kell
#include "../szakdoga/approximator.h"

namespace Utility
{
	struct PriorResult {
		int id;
		float value;
		PriorResult(const int& _id, const float& vol) : id(_id),value(vol) {}
	};

	struct PlaneResult {
		approx::Vector3<float> normal;
		approx::Vector3<float> point;
		PlaneResult(const approx::Vector3<float> n = approx::Vector3<float>(1,0,0) , const approx::Vector3<float>& p = approx::Vector3<float>(0,0,0) ) : normal(n), point(p) {}
	};

	glm::mat4 GetTranslate(const approx::Vector3<float>&, const glm::vec3&, const float&);
	glm::mat4 GetRotateFromNorm(const glm::vec3&);

	glm::vec3 DescartesToPolar(float, float );

	//---------------------------
	template <typename T>
	T GetDeterminantBySarrus(const std::vector< std::vector<T> >& matrix_3x3)
	{
		T sum_one = 0, sum_two = 0;

		for (int i = 0; i < 3; ++i)
		{
			sum_one += matrix_3x3[0][(0 + i) % 3] * matrix_3x3[1][(1 + i) % 3] * matrix_3x3[2][(2 + i) % 3];
			sum_two += matrix_3x3[2][(0 + i ) % 3] * matrix_3x3[1][(1 + i) % 3] * matrix_3x3[0][(2 + i) % 3];
		}

		return sum_one - sum_two;
	}

	/*---------------------------*/

	std::vector< std::vector<int>> GetAdjacencyMatrix(const std::vector<approx::Face<float>>*);

};
