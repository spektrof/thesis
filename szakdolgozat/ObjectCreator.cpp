#include "ObjectCreator.h"

void ObjectCreator::BindingBufferData(GLuint* vao_buffer, GLuint* vbo_buffer, const std::vector<Vertex> vertexes)
{
	//Set activet our 2 buffers
	glBindVertexArray(*vao_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_buffer);

	//Upload our datas				- ezut�n nem �rjuk fel�l �s minden rajzol�sn�l felhaszn�ljuk
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
		// TODO : X , Y , Z Matematikai koord vs. opengl koord + TODO: norm�lisok
		_3DObject.push_back({ glm::vec3(points[i].x, points[i].y, points[i].z)/*, glm::vec3(points[i].x, points[i].y, points[i].z)*//*, glm::vec3(0.0f, 1.0f, 0.0f)*/ });
	}

	BindingBufferData(&_3DvaoID, &_3DvboID, _3DObject);
	BindingBufferIndicies(&index, indexes);

	glBindVertexArray(0);	//vao leszed�se
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszed�se
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszed�se
}
void ObjectCreator::Create2DObject(const std::vector<glm::vec2>& points, GLuint& _2DvaoID, GLuint& _2DvboID)	//be: 2d-s pontok vektorban
{
	std::vector<Vertex> _2DObject;

	for (size_t i = 0; i < points.size(); ++i)
	{
		_2DObject.push_back({ glm::vec3(points[i].y, 0, points[i].x)/*, glm::vec3(0.0f,1.0f,0.0f)*//*, glm::vec3(1.0f, 1.0f, 0.0f)*/ });
	}

	glGenVertexArrays(1, &_2DvaoID); // vao create 
	glGenBuffers(1, &_2DvboID);	//vbo create

	BindingBufferData(&_2DvaoID, &_2DvboID, _2DObject);

	glBindVertexArray(0);	//vao leszed�se
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszed�se
}
void ObjectCreator::CreateCuttingPlane(GLuint& plane_vaoid, GLuint& plane_vboid, GLuint& plane_index) {
	// norm�lisa (0,0,1)
	ObjectCreator::Vertex CuttingPlane[] = { { glm::vec3(-1, 1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0)*/ },
	{ glm::vec3(-1, 0, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */ },
	{ glm::vec3(-1, -1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */ },
	{ glm::vec3(0, -1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */ },
	{ glm::vec3(1, -1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */ },
	{ glm::vec3(1, 0, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */ },
	{ glm::vec3(1, 1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0)*/ },
	{ glm::vec3(0, 1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0)*/ } };

	GLushort Planeindices[] = {
		0, 2, 4,
		4, 6, 0,
		1, 3, 5,
		5, 7, 1 };

	glGenVertexArrays(1, &plane_vaoid); // vao create
	glGenBuffers(1, &plane_vboid);	//vbo create

	BindingBufferData(&plane_vaoid, &plane_vboid, std::vector<Vertex>(CuttingPlane, CuttingPlane + 8));
	BindingBufferIndicies(&plane_index, std::vector<GLushort>(Planeindices, Planeindices + 12));

	glBindVertexArray(0);	//vao leszed�se
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszed�se
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszed�se
}