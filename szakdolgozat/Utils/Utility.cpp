#include "Utility.h"

glm::vec3 Utility::DescartesToPolar(float omega, float theta)
{
	return glm::vec3(cosf(omega)*sinf(theta), cosf(theta), sinf(omega)*sinf(theta));
}

glm::mat4 Utility::GetTranslateFromCoord(glm::vec3 vec,glm::vec3 norm)
{
	//return glm::translate<float>(16.0f, 31.0f, 31.0f);
	//return glm::translate<float>(vec.x, vec.y, vec.z);
	return glm::translate<float>(vec.x + (norm.x != 0.0f ? 1.0f : 0.0f), vec.z + (norm.z != 0.0f ? 1.0f : 0.0f), vec.y + (norm.y != 0.0f ? 1.0f : 0.0f));
}
glm::mat4 Utility::GetRotateFromNorm(glm::vec3 vec)
{
	glm::vec3 axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), vec);

	float angle = glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), vec));
	if (angle == 0.0f)
		return glm::rotate<float>(angle, 1.0f, 0.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - helybenhagyás
	if (angle == (float)M_PI)
		return glm::rotate<float>(angle * 180.0f / (float)M_PI, 0.0f, 1.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - fordulás

	axis = glm::normalize(axis);
	return glm::rotate<float>(angle * 180.0f / (float)M_PI, axis.x, axis.y, axis.z);
}