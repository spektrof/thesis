#pragma once

#include <SDL.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <vector> // pair miatt kell

namespace Utility
{
	typedef std::pair<int, float> data_t;

	struct ResultData {
		int id;
		float value;
		ResultData(const int& _id, const float& vol) : id(_id),value(vol) {}
	};

	
	glm::mat4 GetTranslateFromCoord(glm::vec3, glm::vec3);
	glm::mat4 GetRotateFromNorm(glm::vec3);

	glm::vec3 DescartesToPolar(float, float );


};