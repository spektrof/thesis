#pragma once

/* Keszitette: Lukacs Peter

   Objektumok keszitesehez keszult nevter
*/

#include<vector>
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>

namespace ObjectCreator
{
	struct Vertex
	{
		glm::vec3 p;
	};

	void BindingBufferData(GLuint*, GLuint*, const std::vector<Vertex>);
	void BindingBufferIndicies(GLuint*, const std::vector<GLuint>);

	void CreateCuttingPlane(GLuint&, GLuint&, GLuint&, const int&, const int&);
	void Create2DObject(const std::vector<glm::vec2>&, GLuint& , GLuint&);
	void Create3DObject(const std::vector<glm::vec3>&, const std::vector<unsigned int>&, GLuint&, GLuint&, GLuint&);
};
