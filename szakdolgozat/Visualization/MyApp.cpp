#include "MyApp.h"
#include "../Utils/GLUtils.hpp"

#include <GL/GLU.h>

#include <math.h>
#include <vector>

#define CuttingPlaneWireType 1
#define _3DWireType 0
#define TRANSPARENCY 1

CMyApp::CMyApp(void)
{
	_2DvaoID = 0;
	_3DvaoID = 0;
	plane_vaoid = 0;
	_2DvboID = 0;
	_3DvboID = 0;
	plane_vboid = 0;
	ibID = 0;
	plane_index = 0;
	_3Dindex = 0;

	programID = 0;

	ViewPoint = _3D;

	ActiveAtom = 0;
}

CMyApp::~CMyApp(void)
{

}

//Inicializálás
bool CMyApp::Init()
{
	if (!EngineInit()) { std::cout << "ERROR during engine init!\n"; return false;  }

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);

	// Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	// geometry creating
	CreateCuttingPlane();	// (x,y síkban fekvő , (0,0,1) normálisú négyzet)
	 
	std::vector<glm::vec2> input /* = GetProj2D_Points() */;

	input.push_back(glm::vec2(-1.0f, -2.0f));
	input.push_back(glm::vec2(0.0f, -3.0f));
	input.push_back(glm::vec2(1.0f, 3.0f));

	Create2DObject(input);
	//Ideiglenes3DKocka();
	AddShaders();

	return true;
}

bool CMyApp::EngineInit() 
{
	if (!app.set_target("test.obj", 0.5f)) {
		std::cout << "HIBA A FAJL BETOLTESENEL!\n";
		return false;
	}
	data = app.atom_drawinfo();
	targetdata = app.target_drawinfo();
	Create3DObject(data.points, data.indicies);

	return true;
}

//Rajzolás + Objektumok
void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	//--------------------------------------------------------
	if (transparency && TRANSPARENCY)
			glEnable(GL_BLEND);
	else	glDisable(GL_BLEND);
	//--------------------------------------------------------

	glUniform3f(eyePos, eye.x, eye.y, eye.z);
	glUniform3f(Light, FenyIrany.x, FenyIrany.y, FenyIrany.z);
	glUniform1i(View, ViewPoint == _2D ? 1 : 0);

	//--------------------------------------------------------

	if (ViewPoint)	// true when _2D
	{
		Draw2D(3, glm::scale<float>(3.0f, 3.0f, 3.0f));
	}
	else // _3D
	{
		switch(request.happen) 
		{
			case ACCEPT:
				AcceptCutting();
				break;
			case CUTTING:
				GetResult();
				break;
			case UNDO:
				GetUndo();
				break;
			case RESTART:
				GetRestart();
				break;
			default:
				break;
		}
		
		DrawCuttingPlane(GetTranslateFromCoord(approx::convert(p.example_point())), GetRotateFromNorm(approx::convert(p.normal())), glm::scale<float>(50.0f, 50.0f, 50.0f));
	
		for (int i = 0; i < data.index_ranges.size() - 1; ++i)
		{
			Draw3D(i, 0.4f,1, glm::scale<float>(1.0f, 1.0f, 1.0f));
		}
	}

	glUseProgram(0);

}

void CMyApp::AcceptCutting()
{

	switch (request.ta)
	{ 
		case BOTH:
			app.container().last_cut_result().choose_both();
			
			data = app.atom_drawinfo();
			Create3DObject(data.points, data.indicies);
			break;
		case POSITIVE:
			app.container().last_cut_result().choose_positive();

			data = app.atom_drawinfo();
			Create3DObject(data.points, data.indicies);
			break;
		case NEGATIVE:
			app.container().last_cut_result().choose_negative();

			data = app.atom_drawinfo();
			Create3DObject(data.points, data.indicies);
			break;
	}
}

void CMyApp::GetResult()
{
	NumberOfAtoms++;
	app.container().cut(0, p);

	data = app.cut_drawinfo();
	Create3DObject(data.points, data.indicies);
}

void CMyApp::GetUndo()
{
	NumberOfAtoms--;
	app.container().last_cut_result().undo();
	data = app.atom_drawinfo();
	Create3DObject(data.points, data.indicies);
}

void CMyApp::GetRestart()
{
	NumberOfAtoms = 1;
	ActiveAtom = 0;
	app.restart();
	data = app.atom_drawinfo();
	Create3DObject(data.points, data.indicies);
}

void CMyApp::BindingBufferData(GLuint* vao_buffer, GLuint* vbo_buffer, const std::vector<Vertex> vertexes)
{
	//Set activet our 2 buffers
	glBindVertexArray(*vao_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_buffer);

	//Upload our datas				- ezután nem írjuk felül és minden rajzolásnál felhasználjuk
	glBufferData(GL_ARRAY_BUFFER, vertexes.size()*sizeof(Vertex), &vertexes[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	/*glEnableVertexAttribArray(1);	//normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));*/

	/*glEnableVertexAttribArray(2);	//cols
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) * 2));*/

}
void CMyApp::BindingBufferIndicies(GLuint* ind_buffer, const std::vector<GLushort> indicies)
{
	glGenBuffers(1, &plane_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size()*sizeof(GLushort), &indicies[0], GL_STATIC_DRAW);
}

void CMyApp::AddShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "Shaders/myVert.vert");
	GLuint gs_ID = loadShader(GL_GEOMETRY_SHADER, "Shaders/geometry_shader.geom");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "Shaders/myFrag.frag");

	programID = glCreateProgram();

	glAttachShader(programID, vs_ID);
	glAttachShader(programID, gs_ID);
	glAttachShader(programID, fs_ID);

	glBindAttribLocation(programID, 0, "vs_in_pos");
	//glBindAttribLocation(programID, 1, "vs_in_normal");
	//glBindAttribLocation(programID, 2, "vs_in_col");

	glLinkProgram(programID);

	glDeleteShader(vs_ID);
	glDeleteShader(gs_ID);
	glDeleteShader(fs_ID);

	m_matProj = glm::perspective(45.0f, 800 / 600.0f, 0.01f, 1000.0f);

	AddShaderUniformLocations();

}
void CMyApp::AddShaderUniformLocations()
{
	m_loc_mvp = glGetUniformLocation(programID, "MVP");
	m_loc_world = glGetUniformLocation(programID, "world");
	m_loc_worldIT = glGetUniformLocation(programID, "worldIT");
	eyePos = glGetUniformLocation(programID, "EyePosition");
	Light = glGetUniformLocation(programID, "LightDirection");
	View = glGetUniformLocation(programID, "View");
	Opacity = glGetUniformLocation(programID, "opacity");
}

void CMyApp::Create3DObject(const std::vector<glm::vec3>& points, const std::vector<unsigned short>& indexes)
{
	glGenVertexArrays(1, &_3DvaoID); // vao create
	glGenBuffers(1, &_3DvboID);	//vbo create

	std::vector<Vertex> _3DObject;

	for (size_t i = 0; i < points.size(); ++i)
	{
							// TODO : X , Y , Z Matematikai koord vs. opengl koord + TODO: normálisok
		_3DObject.push_back({ glm::vec3(points[i].x, points[i].z, points[i].y)/*, glm::vec3(points[i].x, points[i].y, points[i].z)*//*, glm::vec3(0.0f, 1.0f, 0.0f)*/ });
	}

	BindingBufferData(&_3DvaoID, &_3DvboID, _3DObject);
	BindingBufferIndicies(&plane_index, indexes);

	glBindVertexArray(0);	//vao leszedése
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedése
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszedése
}
void CMyApp::Draw3D(const int& which, const float& _opacity, const bool& backdropping, glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
{
	if (backdropping)	
	{	
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	glPolygonMode(GL_FRONT, _3DWireType ? GL_LINE : GL_FILL);

	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	glm::mat4 mvp = m_matProj * m_matView * matWorld;
	
	glBindVertexArray(_3DvaoID);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(matWorld[0][0]));
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &(matWorldIT[0][0]));
	
	glUniform1f(Opacity, ActiveAtom == which ? 1.0f : _opacity);

	int start = data.index_ranges[ which];
	int end = data.index_ranges[ which +1];

	glDrawElements(GL_TRIANGLES,		// primitív típus
		end - start,		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		(void*)(start * sizeof(unsigned short)));					// indexek cime
}

void CMyApp::Create2DObject(const std::vector<glm::vec2>& points )	//be: 2d-s pontok vektorban
{
	// Kör - teszt
	/*Vertex _2DObject[22];

	_2DObject[0] = { glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 0.0f) };
	for (int i = 0; i <= 20; ++i)
	{
		float a = i * 2.0f * 3.14152f / 20; //radian
		float x = 3 * cosf(a);
		float y = 3 * sinf(a);
		_2DObject[i + 1] = { glm::vec3(y, 0, x), glm::vec3(1.0f, 1.0f, 0.0f) };
	}*/

	// Result
	std::vector<Vertex> _2DObject;

	for (size_t i = 0; i < points.size(); ++i)
	{
		_2DObject.push_back({ glm::vec3(points[i].y, 0, points[i].x)/*, glm::vec3(0.0f,1.0f,0.0f)*//*, glm::vec3(1.0f, 1.0f, 0.0f)*/ });
	}

	glGenVertexArrays(1, &_2DvaoID); // vao create , ha null akkor vmilyen adat nem jo ?? :/
	glGenBuffers(1, &_2DvboID);	//vbo create

	//BindingBufferData(&_2DvaoID, &_2DvboID, std::vector<Vertex>(_2DObject, _2DObject + 22)); //kor
	BindingBufferData(&_2DvaoID, &_2DvboID, _2DObject);

	glBindVertexArray(0);	//vao leszedése
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedése
}
void CMyApp::Draw2D(const int& NumbersOfVertices, glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
{
	//glPolygonMode(GL_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, GL_FILL);
	
	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glBindVertexArray(_2DvaoID);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(matWorld[0][0]));
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &(matWorldIT[0][0]));

	glDrawArrays(GL_TRIANGLE_FAN,
		0,
		NumbersOfVertices);
}

void CMyApp::CreateCuttingPlane(){
	// normálisa (0,0,1)
	Vertex CuttingPlane[] = { { glm::vec3(-1, 1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0)*/ },
	{ glm::vec3(-1, 0, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */},
	{ glm::vec3(-1, -1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */},
	{ glm::vec3(0, -1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */},
	{ glm::vec3(1, -1, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */},
	{ glm::vec3(1, 0, 0)/*, glm::vec3(0, 0, 1)*//*, glm::vec3(1, 0, 0) */},
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

	glBindVertexArray(0);	//vao leszedése
	glBindBuffer(GL_ARRAY_BUFFER, 0);	//vbo leszedése
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	//index leszedése
}
void CMyApp::DrawCuttingPlane(glm::mat4& trans, glm::mat4& rot, glm::mat4& scal)
{
	glPolygonMode(GL_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, CuttingPlaneWireType ? GL_LINE : GL_FILL);	

	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glBindVertexArray(plane_vaoid);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(matWorld[0][0]));
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &(matWorldIT[0][0]));


	glDrawElements(GL_TRIANGLES,		
		12,		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime
}

void CMyApp::Update()
{
	m_matView = glm::lookAt(eye, at, up);
	// honnan,  melyik pont, melyik irány (most felfelé)
	request = ui.GetRequest();
}

//Mouse/Keyboard eventek
void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	glm::vec3 sd = ViewPoint == _2D ? glm::vec3(0,0,-1) : glm::normalize(glm::cross(up, fw));
	glm::vec3 ld = glm::normalize(glm::cross(up, lw));

	switch (key.keysym.sym){
	case SDLK_t:	//opacity
		transparency = !transparency;
		break;
	case SDLK_p:	// change projections
		ViewPoint = ViewPoint ? _3D : _2D;
		if (ViewPoint) // _2D
		{
			fw = glm::vec3(1, 0, 0);
			eye = glm::vec3(0, 50, 0);
			up = glm::vec3(1, 0, 0);
			at = glm::vec3(0, 0, 0);
			//Getter
			//Create2DObject();
		}
		else
		{ //3D
			u = 4.0f; v = 2.0f; fw = ToDescartes(u, v);
			eye = glm::vec3(50, 35, 50);
			up = glm::vec3(0, 1, 0);
			at = eye + fw;
		}
		break;
	case SDLK_i: // info
		GetInfo();
		break;
	case SDLK_w:	//camera
		eye +=  fw;
		at +=  fw;
		break;
	case SDLK_s:
		eye -= fw;
		at -= fw;
		break;
	case SDLK_a:
		eye += sd;
		at += sd;
		break;
	case SDLK_d:
		eye -= sd;
		at -= sd;
		break;
	case SDLK_DOWN:	//feny
		FenyIrany -= lw;
		break;
	case SDLK_UP:
		FenyIrany += lw;
		break;
	case SDLK_LEFT:
		FenyIrany -= ld;
		break;
	case SDLK_RIGHT:
		FenyIrany += ld;
		break;
	case SDLK_KP_PLUS:	//atomváltás
		ActiveAtom = (ActiveAtom + 1) % NumberOfAtoms;
		break;
	case SDLK_KP_MINUS:
		ActiveAtom = (ActiveAtom - 1) % NumberOfAtoms;	
		break;
	}
}
void CMyApp::KeyboardUp(SDL_KeyboardEvent&)
{

}
void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	if (ViewPoint == _2D) return;

	if (is_left_pressed){
		u += mouse.xrel / 100.0f;
		v += mouse.yrel / 100.0f;

		v = glm::clamp(v, 0.1f, 3.0f);

		fw = ToDescartes(u, v);
		at = eye + fw;
	}
}
void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_LEFT){
		is_left_pressed = true;
	}
}
void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_LEFT)
	{
		is_left_pressed = false;
	}
}
void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
	wheel.y > 0 ? (eye += fw, at += fw) : (eye -= fw, at -= fw);
}

//Egyéb
void CMyApp::Clean()
{
	glDeleteBuffers(1, &plane_vaoid);
	glDeleteBuffers(1, &plane_index);
	glDeleteVertexArrays(1, &plane_vboid);

	glDeleteBuffers(1, &_2DvaoID);
	glDeleteVertexArrays(1, &_2DvboID);

	glDeleteBuffers(1, &_3DvaoID);
	glDeleteBuffers(1, &_3Dindex);
	glDeleteVertexArrays(1, &_3DvboID);

	glDeleteProgram(programID);
}
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	// nyilasszog, ablakmeret nezeti arany, kozeli és tavoli vagosik
	m_matProj = glm::perspective(45.0f, _w / (float)_h, 0.01f, 1000.0f);
}

//segédfvek
glm::vec3 CMyApp::ToDescartes(float u, float v)
{
	return glm::vec3(cosf(u)*sinf(v), cosf(v), sinf(u)*sinf(v));
}
glm::mat4 CMyApp::GetTranslateFromCoord(glm::vec3 vec)
{
	return glm::translate<float>(vec.x+1.0f, -vec.z, -vec.y);
}
glm::mat4 CMyApp::GetRotateFromNorm(glm::vec3 vec)
{
	glm::vec3 axis = glm::cross( glm::vec3(0.0f,0.0f,1.0f), vec);

	float angle = glm::acos( glm::dot( glm::vec3(0.0f,0.0f,1.0f), vec) );
	if (angle == 0.0f) 
		return glm::rotate<float>(angle, 1.0f, 0.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - helybenhagyás
	if (angle == (float)M_PI) 
		return glm::rotate<float>(angle * 180.0f / (float)M_PI, 0.0f, 1.0f, 0.0f); // ekkor a keresztszorzat 0 -> nem lehet normalizálni - fordulás

	axis = glm::normalize(axis);
	return glm::rotate<float>(angle * 180.0f / (float)M_PI, axis.x, axis.y, axis.z);
}

void CMyApp::GetInfo() {
	std::cout << "A celtest terfogata: " << app.target().body().volume() << "\n";
	std::cout << "negativ oldali keletkezett atom terfogata: " << app.container().last_cut_result().negative()->volume() << "\n";
	std::cout << "pozitiv oldali keletkezett atom terfogata: " << app.container().last_cut_result().positive()->volume() << "\n";
	 

}

void CMyApp::Ideiglenes3DKocka()
{
	std::vector<glm::vec3> input2;
	std::vector<unsigned short> indexes = {
		0, 2, 1,	//baloldal
		2, 3, 1,
		0, 4, 6,	//teteje
		6, 2, 0,
		0, 1, 5,	//szembe
		5, 4, 0,
		7, 5, 1,	//alja
		1, 3, 7,
		4, 5, 7, //jobboldal
		7, 6, 4,
		7, 3, 2,	//hátul
		2, 6, 7
	};
	CountsOfIndexes = (GLuint)indexes.size();

	input2.push_back(glm::vec3(-1.0f, 1.0f, 1.0f));
	input2.push_back(glm::vec3(-1.0f, -1.0f, 1.0f));
	input2.push_back(glm::vec3(-1.0f, 1.0f, -1.0f));
	input2.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
	input2.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	input2.push_back(glm::vec3(1.0f, -1.0f, 1.0f));
	input2.push_back(glm::vec3(1.0f, 1.0f, -1.0f));
	input2.push_back(glm::vec3(1.0f, -1.0f, -1.0f));

	Create3DObject(input2, indexes);

}

// glPolygonMode - mode : GL_POINT,GL_FILL,GL_LINE
/*
//_-----------------------
//Draw3D(CountsOfIndexes, glm::scale<float>(5.0f, 5.0f, 5.0f), glm::translate<float>(10.0f * NumberOfAtoms, 0.0f, 0.0f));
//Draw3D(CountsOfIndexes, glm::scale<float>(5.0f, 5.0f, 5.0f), glm::translate<float>(10.0f * NumberOfAtoms+12.0f, 0.0f, 0.0f));

*/