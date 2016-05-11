#include "ObjectCreator.h"

/*Adatfeltoltes*/
void ObjectCreator::BindingBufferData(GLuint* vao_buffer, GLuint* vbo_buffer, const std::vector<Vertex> vertexes)
{
	//Aktivizaljuk a ket buffert
	glBindVertexArray(*vao_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_buffer);

	//Feltoltjuk az adatainkat
	glBufferData(GL_ARRAY_BUFFER, vertexes.size()*sizeof(Vertex), &vertexes[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
}

/*Indexfeltoltes*/
void ObjectCreator::BindingBufferIndicies(GLuint* ind_buffer, const std::vector<GLuint> indicies)
{
	glGenBuffers(1, ind_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ind_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size()*sizeof(GLuint), &indicies[0], GL_STATIC_DRAW);
}

/*3D objektum keszites*/
void ObjectCreator::Create3DObject(const std::vector<glm::vec3>& points, const std::vector<unsigned int>& indexes, GLuint& _3DvaoID, GLuint& _3DvboID, GLuint& index)
{
	glGenVertexArrays(1, &_3DvaoID); // vao keszites
	glGenBuffers(1, &_3DvboID);	//vbo keszites

	std::vector<Vertex> _3DObject;

	for (std::vector<glm::vec3>::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		_3DObject.push_back({ *it });
	}

	BindingBufferData(&_3DvaoID, &_3DvboID, _3DObject);
	BindingBufferIndicies(&index, indexes);

	glBindVertexArray(0);	//vao leszedese
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedese
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszedese
}

/*2D objektum keszites*/
void ObjectCreator::Create2DObject(const std::vector<glm::vec2>& points, GLuint& _2DvaoID, GLuint& _2DvboID)	//be: 2d-s pontok vektorban
{
	std::vector<Vertex> _2DObject;

	for (std::vector<glm::vec2>::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		_2DObject.push_back({ glm::vec3(it->x, 0, it->y) });
	}

	glGenVertexArrays(1, &_2DvaoID);
	glGenBuffers(1, &_2DvboID);	

	BindingBufferData(&_2DvaoID, &_2DvboID, _2DObject);

	glBindVertexArray(0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*Vagosik keszites*/
void ObjectCreator::CreateCuttingPlane(GLuint& plane_vaoid, GLuint& plane_vboid, GLuint& plane_index, const int& size, const int& freq) {
	// normalisa (0,0,1)
	std::vector<ObjectCreator::Vertex> CuttingPlane;
	
	//negyzetenkent, bal felso sarokbol indulunk
	float distance = (float)size / (float)freq;
	for (int i = 0; i <= freq; ++i)
	{
		for (int j = 0; j <= freq; ++j)
		{
			CuttingPlane.push_back({ glm::vec3(-size/2 + i * distance , -size/2 + j * distance, 0.0f) });
		}
	}

	std::vector<GLuint> Planeindices;

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

	glGenVertexArrays(1, &plane_vaoid); 
	glGenBuffers(1, &plane_vboid);

	BindingBufferData(&plane_vaoid, &plane_vboid, CuttingPlane);
	BindingBufferIndicies(&plane_index, Planeindices);

	glBindVertexArray(0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
}