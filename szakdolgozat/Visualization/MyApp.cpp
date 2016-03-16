#include "MyApp.h"
#include "../Utils/GLUtils.hpp"

#include <GL/GLU.h>

#include <math.h>
#include <vector>

#define CuttingPlaneWireType 1
#define _3DWireType 0
#define TRANSPARENCY 1

Visualization::Visualization(void)
{
	_2DvaoID = 0;
	_2DvboID = 0;

	_3DvaoID = 0;
	_3DvboID = 0;
	_3Dindex = 0;

	plane_vaoid = 0;
	plane_vboid = 0;
	plane_index = 0;

	_target_vaoID = 0;
	_target_vboID = 0;
	_target_indexID = 0;
	
	programID = 0;

	ActiveAtom = 0;
}

Visualization::~Visualization(void)
{

}

//Inicializálás
bool Visualization::Init()
{
	if (!EngineInit()) { std::cout << "ERROR during engine init!\n"; return false;  }

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);
	// Enable blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	// geometry creating
	ObjectCreator::CreateCuttingPlane(plane_vaoid,plane_vboid,plane_index);	// (x,y síkban fekvő , (0,0,1) normálisú négyzet)
	 
	std::vector<glm::vec2> input /* = GetProj2D_Points() */;

	input.push_back(glm::vec2(-1.0f, -2.0f));
	input.push_back(glm::vec2(0.0f, -3.0f));
	input.push_back(glm::vec2(1.0f, 3.0f));

	ObjectCreator::Create2DObject(input,_2DvaoID,_2DvboID);
	//Ideiglenes3DKocka();
	AddShaders();

	return true;
}

bool Visualization::EngineInit() 
{
	if (!app.set_target("test.obj", 0.5f)) {
		std::cout << "HIBA A FAJL BETOLTESENEL!\n";
		return false;
	}
	data = app.atom_drawinfo();
	targetdata = app.target_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
	ObjectCreator::Create3DObject(targetdata.points, targetdata.indicies,_target_vaoID,_target_vboID, _target_indexID);

	return true;
}

//Rajzolás + Objektumok
void Visualization::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	//--------------------------------------------------------
	if (transparency && TRANSPARENCY)
			glEnable(GL_BLEND);
	else	glDisable(GL_BLEND);
	//--------------------------------------------------------

	glUniform3fv(eyePos, 1, glm::value_ptr(c.GetEye())); //jó ez? :DD
	glUniform3fv(Lights,1, glm::value_ptr(l.GetLightDir()));
	glUniform1i(View, c.GetView() );

	//--------------------------------------------------------

	if (c.Is2DView())	// true when _2D
	{
		Draw2D(3, glm::scale<float>(3.0f, 3.0f, 3.0f));
	}
	else // _3D
	{
		switch (request.happen)
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
		case NEWPLANE:
			SetNewPlane();
			break;
		case RESTART:
			GetRestart();
			break;
		default:
			break;
		}

		DrawCuttingPlane(Utility::GetTranslateFromCoord(approx::convert(p.example_point()), approx::convert(p.normal())), Utility::GetRotateFromNorm(approx::convert(p.normal())), glm::scale<float>(50.0f, 50.0f, 50.0f));
		//DrawTargetBody();

		/*	DRAWING METHOD:
				1.Draw all opaque objects first. -> amik nem átlátszók
				2.Sort all the transparent objects.	-> z buffer szerinti sorbarendezés , legbelsőt először
				3.Draw all the transparent objects in sorted order.	-> a sorrend szerinti rajz
		*/
		SortAlphaBlendedObjects(data);
		for (std::deque<Utility::data_t>::iterator it = SortedObjects.begin() ; it < SortedObjects.end() ; ++it)
		{

		Draw3D(it->first, 0.6f, 1, glm::scale<float>(1.0f, 1.0f, 1.0f));
		
		}
		/*glClear(GL_ACCUM_BUFFER_BIT);
		for (int i = 0; i < data.index_ranges.size() - 1;++i)
		{
			Draw3D(i, 0.6f, 1, glm::scale<float>(1.0f, 1.0f, 1.0f));
			//glAccum(i ? GL_ACCUM : GL_LOAD, 0.5);
		}
		//Draw3D(it->first, 0.4f, 1, glm::scale<float>(0.5f, 0.5f, 0.5f), glm::translate<float>(15.0f, 15.0f, 15.0f));

		//glAccum(i ? GL_ACCUM : GL_LOAD, 0.25);
		//glAccum(GL_RETURN, 1);
		//SwapBuffers(hdc);*/

		
	}

	glUseProgram(0);

}

void Visualization::AcceptCutting()
{
	cutting_mode = false;

	switch (request.ta)
	{ 
		case BOTH:
			NumberOfAtoms++;
			app.container().last_cut_result().choose_both();
			
			data = app.atom_drawinfo();
			ObjectCreator::Create3DObject(data.points, data.indicies,_3DvaoID,_3DvboID, _3Dindex);
			break;
		case POSITIVE:
			app.container().last_cut_result().choose_positive();

			data = app.atom_drawinfo();
			ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
			break;
		case NEGATIVE:
			app.container().last_cut_result().choose_negative();

			data = app.atom_drawinfo();
			ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
			break;
	}

	//TODO szar mikor vágás történik -> deque iterator nem talál semmit
	request.ta = NEGATIVE;

}

void Visualization::GetResult()
{
	cutting_mode = true;
	
	app.container().cut(0, p);

	data = app.cut_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
}

void Visualization::GetUndo()
{
	NumberOfAtoms--;
	app.container().last_cut_result().undo();
	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
}

void Visualization::GetRestart()
{
	NumberOfAtoms = 1;
	ActiveAtom = 0;
	app.restart();
	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
}

void Visualization::SetNewPlane()
{
	p = approx::Plane<float>(
							 { request.plane_norm.x, request.plane_norm.y, request.plane_norm.z },
		approx::Vector3<float>(request.plane_coord.x, request.plane_coord.y, request.plane_coord.z));
}

void Visualization::AddShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "Shaders/myVert.vert");
	GLuint gs_ID = loadShader(GL_GEOMETRY_SHADER, "Shaders/geometry_shader.geom");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "Shaders/myFrag.frag");

	programID = glCreateProgram();

	glAttachShader(programID, vs_ID);
	glAttachShader(programID, gs_ID);
	glAttachShader(programID, fs_ID);

	glBindAttribLocation(programID, 0, "vs_in_pos");
	
	glLinkProgram(programID);

	glDeleteShader(vs_ID);
	glDeleteShader(gs_ID);
	glDeleteShader(fs_ID);

	m_matProj = glm::perspective(45.0f, 800 / 600.0f, 0.01f, 1000.0f);

	AddShaderUniformLocations();

}
void Visualization::AddShaderUniformLocations()
{
	m_loc_mvp = glGetUniformLocation(programID, "MVP");
	eyePos = glGetUniformLocation(programID, "EyePosition");
	Lights = glGetUniformLocation(programID, "LightDirection");
	View = glGetUniformLocation(programID, "View");
	Opacity = glGetUniformLocation(programID, "opacity");
}

void Visualization::Draw3D(const int& which, const float& _opacity, const bool& backdropping, glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
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
	//glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	glm::mat4 mvp = m_matProj * m_matView * matWorld;
	
	glBindVertexArray(_3DvaoID);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	
	glUniform1f(Opacity, IsItActive(which) ? 1.0f : _opacity);

	int start = data.index_ranges[ which];
	int end = data.index_ranges[ which +1];

	glDrawElements(GL_TRIANGLES,		// primitív típus
		end - start,		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		(void*)(start * sizeof(unsigned short)));					// indexek cime
}

void Visualization::DrawTargetBody()
{
	glPolygonMode(GL_FRONT, _3DWireType ? GL_LINE : GL_FILL);

	glBindVertexArray(_target_vaoID);

	glm::mat4 mvp = m_matProj * m_matView * glm::mat4(1);
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

	glUniform1f(Opacity,  1.0f );

	glDrawElements(GL_TRIANGLES,		// primitív típus
		(GLsizei)targetdata.indicies.size(),		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime
}

void Visualization::Draw2D(const int& NumbersOfVertices, glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
{
	//glPolygonMode(GL_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, GL_FILL);
	
	glm::mat4 matWorld = trans * rot * scal;

	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glBindVertexArray(_2DvaoID);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

	glDrawArrays(GL_TRIANGLE_FAN,
		0,
		NumbersOfVertices);
}

void Visualization::DrawCuttingPlane(glm::mat4& trans, glm::mat4& rot, glm::mat4& scal)
{
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, CuttingPlaneWireType ? GL_LINE : GL_FILL);	

	glm::mat4 matWorld = trans * rot * scal;

	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glBindVertexArray(plane_vaoid);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));


	glDrawElements(GL_TRIANGLES,		
		12,		// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,	// indexek tipusa
		0);					// indexek cime
}

void Visualization::Update()
{
	m_matView = glm::lookAt(c.GetEye(), c.GetAt(), c.GetUp());
	// honnan,  melyik pont, melyik irány (most felfelé)
	request = ui.GetRequest();
}

//Mouse/Keyboard eventek
void Visualization::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym){
	case SDLK_t:	//opacity
		transparency = !transparency;
		break;
	case SDLK_p:	// change projections
		c.SwitchCameraView();
		break;
	case SDLK_i: // info
		GetInfo();
		break;
	case SDLK_w:	//camera
		c.Add(c.GetVertUnit());
		break;
	case SDLK_s:
		c.Sub(c.GetVertUnit());
		break;
	case SDLK_a:
		c.Add(c.GetCameraUnit());
		break;
	case SDLK_d:
		c.Sub(c.GetCameraUnit());
		break;
	case SDLK_DOWN:	//feny
		l.Sub(c.GetVertUnit());
		break;
	case SDLK_UP:
		l.Add(c.GetVertUnit());
		break;
	case SDLK_LEFT:
		l.Sub(l.GetLightUnit(c.GetUp()));
		break;
	case SDLK_RIGHT:
		l.Add(l.GetLightUnit(c.GetUp()));
		break;
	case SDLK_KP_PLUS:	//atomváltás
		if (!cutting_mode) ActiveAtom = (ActiveAtom + 1) % NumberOfAtoms;
		break;
	case SDLK_KP_MINUS:
		if (!cutting_mode) ActiveAtom = (ActiveAtom - 1) % NumberOfAtoms;
		break;
	}
}
void Visualization::KeyboardUp(SDL_KeyboardEvent&)
{

}
void Visualization::MouseMove(SDL_MouseMotionEvent& mouse)
{
	c.MouseMove(mouse);
}
void Visualization::MouseDown(SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_LEFT){
		c.SetIsLeftPressed(true);
	}
}
void Visualization::MouseUp(SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_LEFT)
	{
		c.SetIsLeftPressed(false);
	}
}
void Visualization::MouseWheel(SDL_MouseWheelEvent& wheel)
{
	wheel.y > 0 ? (c.Add(c.GetVertUnit())) : (c.Sub(c.GetVertUnit()));
}

//Egyéb
void Visualization::Clean()
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
void Visualization::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	// nyilasszog, ablakmeret nezeti arany, kozeli és tavoli vagosik
	m_matProj = glm::perspective(45.0f, _w / (float)_h, 0.01f, 1000.0f);
}

bool Visualization::IsItActive(const int& which)
{
	return	(request.ta == NEGATIVE && ActiveAtom == which)
		|| (request.ta == POSITIVE && ActiveAtom + 1 == which)
		|| (request.ta == BOTH && (ActiveAtom + 1 == which || ActiveAtom == which));
}
void Visualization::GetInfo()
{
	std::cout << "A celtest terfogata: " << app.target().body().volume() << "\n";
	std::cout << "negativ oldali keletkezett atom terfogata: " << app.container().last_cut_result().negative()->volume() << "\n";
	std::cout << "pozitiv oldali keletkezett atom terfogata: " << app.container().last_cut_result().positive()->volume() << "\n";
}

//objektumonkénti rendezés, pontok alapján
/* TODO HACK : mikor vágunk positive vagy bothba akkor is lefut viszont értelmetlenné váli ka vége (Both, positive vizsgálat)*/
void Visualization::SortAlphaBlendedObjects(approx::BodyList& data)
{
	SortedObjects.clear();
	
	for (int i = 0; i < data.index_ranges.size() - 1; i++)
	{
		int start = data.index_ranges[i];
		int end = data.index_ranges[i+1];
		int length = (end - start) / 3;

		std::vector<float> sps;
		sps.clear();
		for (int ObjectTriangles = start; ObjectTriangles < end ; ++ObjectTriangles)
		{
			/*glm::vec3 a = data.points[ data.indicies[start + 3 * ObjectTriangles ] ];	//súlypont
			glm::vec3 b = data.points[ data.indicies[start + 3 * ObjectTriangles + 1] ];
			glm::vec3 c = data.points[ data.indicies[start + 3 * ObjectTriangles + 2] ];
			glm::vec3 sp = glm::vec3( (a.x + b.x + c.x)/ 3.0f , (a.y + b.y + c.y) / 3.0f , (a.z + b.z + c.z) / 3.0f);*/
			glm::vec3 sp = data.points[data.indicies[ObjectTriangles]];
			sps.push_back( glm::dot( c.GetEye() - sp , c.GetEye() -sp) );
		}
		auto result = *std::min_element(std::begin(sps), std::end(sps));
		SortedObjects.push_back(Utility::data_t(i, result));
	}

	std::sort(SortedObjects.begin(), SortedObjects.end(), Utility::greater_second<Utility::data_t>());
	std::deque<Utility::data_t>::iterator temp;
	Utility::data_t temp_data;
	if (request.ta == BOTH) {
		temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom+1 ; });
		temp_data = Utility::data_t(temp->first, temp->second);
		SortedObjects.erase(temp);
		SortedObjects.push_front(temp_data);

		temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom; });
		temp_data = Utility::data_t(temp->first, temp->second);
		SortedObjects.erase(temp);
		SortedObjects.push_front(temp_data);

	}
	else if (request.ta == POSITIVE){
		temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom + 1; });
		temp_data = Utility::data_t(temp->first, temp->second);
		SortedObjects.erase(temp);
		SortedObjects.push_front(temp_data);
	}
	else {
		temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom; });
		temp_data = Utility::data_t(temp->first, temp->second);
		SortedObjects.erase(temp);
		SortedObjects.push_front(temp_data);
	}
	
	
}