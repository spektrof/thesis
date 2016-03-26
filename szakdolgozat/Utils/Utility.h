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
	typedef std::pair<int, float> data_t;

	struct PriorResult {
		int id;
		float value;
		PriorResult(const int& _id, const float& vol) : id(_id),value(vol) {}
	};

	struct Touch {
		approx::ConvexAtom<float> atom;
		int value;
		Touch(const approx::ConvexAtom<float> a, const int& v) : atom(a),value(v) {}
	};

	struct PlaneResult {
		approx::Vector3<float> normal;
		approx::Vector3<float> point;
		PlaneResult(const approx::Vector3<float> n = approx::Vector3<float>(0,0,1) , const approx::Vector3<float>& p = approx::Vector3<float>(0,0,26) ) : normal(n), point(p) {}
	};

	struct writer
	{
		bool operator()(const Utility::PriorResult& a)
		{
			std::cout << a.id << " " << a.value << "\n";
			return true;
		}
	};

	glm::mat4 GetTranslateFromCoord(glm::vec3, glm::vec3);
	glm::mat4 GetRotateFromNorm(glm::vec3);

	glm::vec3 DescartesToPolar(float, float );


};