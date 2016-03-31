#include "ObjectCreator.h"

void ObjectCreator::BindingBufferData(GLuint* vao_buffer, GLuint* vbo_buffer, const std::vector<Vertex> vertexes)
{
	//Set activet our 2 buffers
	glBindVertexArray(*vao_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_buffer);

	//Upload our datas				- ezután nem írjuk felül és minden rajzolásnál felhasználjuk
	glBufferData(GL_ARRAY_BUFFER, vertexes.size()*sizeof(Vertex), &vertexes[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);


}
void ObjectCreator::BindingBufferIndicies(GLuint* ind_buffer, const std::vector<GLushort> indicies)
{
	glGenBuffers(1, ind_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ind_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size()*sizeof(GLushort), &indicies[0], GL_STATIC_DRAW);
}

 
void ObjectCreator::Create3DObject(const std::vector<glm::vec3>& points, const std::vector<unsigned short>& indexes, GLuint& _3DvaoID, GLuint& _3DvboID, GLuint& index)
{
	glGenVertexArrays(1, &_3DvaoID); // vao create
	glGenBuffers(1, &_3DvboID);	//vbo create

	std::vector<Vertex> _3DObject;

	for (size_t i = 0; i < points.size(); ++i)
	{
		// TODO : X , Y , Z Matematikai koord vs. opengl koord + TODO: normálisok
		_3DObject.push_back({ glm::vec3(points[i].x, points[i].y, points[i].z)/*, glm::vec3(points[i].x, points[i].y, points[i].z)*//*, glm::vec3(0.0f, 1.0f, 0.0f)*/ });
	}

	BindingBufferData(&_3DvaoID, &_3DvboID, _3DObject);
	BindingBufferIndicies(&index, indexes);

	glBindVertexArray(0);	//vao leszedése
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedése
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszedése
}
void ObjectCreator::Create2DObject(const std::vector<glm::vec2>& points, GLuint& _2DvaoID, GLuint& _2DvboID)	//be: 2d-s pontok vektorban
{
	std::vector<Vertex> _2DObject;

	for (size_t i = 0; i < points.size(); ++i)
	{
		_2DObject.push_back({ glm::vec3(points[i].x, 0, points[i].y)/*, glm::vec3(0.0f,1.0f,0.0f)*//*, glm::vec3(1.0f, 1.0f, 0.0f)*/ });
	}

	glGenVertexArrays(1, &_2DvaoID); // vao create 
	glGenBuffers(1, &_2DvboID);	//vbo create

	BindingBufferData(&_2DvaoID, &_2DvboID, _2DObject);

	glBindVertexArray(0);	//vao leszedése
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedése
}
void ObjectCreator::CreateCuttingPlane(GLuint& plane_vaoid, GLuint& plane_vboid, GLuint& plane_index, const int& size, const int& freq) {
	// normálisa (0,0,1)
	std::vector<ObjectCreator::Vertex> CuttingPlane;
	
	float distance = (float)size / (float)freq;
	for (int i = 0; i <= freq; ++i)
	{
		for (int j = 0; j <= freq; ++j)
		{
			CuttingPlane.push_back({ glm::vec3(-size/2 + i * distance , -size/2 + j * distance, 0.0f) });
		}
	}

	std::vector<GLushort> Planeindices;

	for (int i = 0; i < freq; ++i)
	{
		for (int j = 0; j < freq; ++j)
		{
			Planeindices.push_back(i*(freq + 1) + j);
			Planeindices.push_back((i+1)*(freq+1) + j);
			Planeindices.push_back((i + 1)*(freq + 1) + j + 1);
			Planeindices.push_back((i + 1)*(freq + 1) + j + 1);
			Planeindices.push_back(i*(freq + 1) + j + 1);
			Planeindices.push_back(i*(freq + 1) + j);
		}
	}

	glGenVertexArrays(1, &plane_vaoid); // vao create
	glGenBuffers(1, &plane_vboid);	//vbo create

	BindingBufferData(&plane_vaoid, &plane_vboid, CuttingPlane);
	BindingBufferIndicies(&plane_index, Planeindices);

	glBindVertexArray(0);	//vao leszedése
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedése
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszedése
}