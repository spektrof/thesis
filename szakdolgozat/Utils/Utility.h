#pragma once

/* Keszitette: Lukacs Peter

   Az egyeb segedtipusokat, fuggvenyeket tartalmazo nevter.
   Azokat tartalmazza, amiket nem art ha tobb helyrol el tudjuk erni.
*/
#include <SDL.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <vector>
#include "../Engine/approximator.h"

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

	glm::vec3 DescartesToPolar(const float&, const float&, const float&);

	std::vector< std::vector<int>> GetAdjacencyMatrix(const std::vector<approx::Face<float>>*);

};