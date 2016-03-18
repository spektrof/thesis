#include "MyApp.h"
#include "../Utils/GLUtils.hpp"

#include <GL/GLU.h>

#include <math.h>
#include <vector>
#include <set>

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
	_tmpActiveAtom = 0;
	//CuttingPlaneFreq = 10;
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
	ObjectCreator::CreateCuttingPlane(plane_vaoid,plane_vboid,plane_index,10, CuttingPlaneFreq);	// (x,y síkban fekvő , (0,0,1) normálisú négyzet)
	 
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

	centr = app.container().atoms(ActiveAtom).centroid();
	distance = p.classify_point(centr);
	_planenormal = approx::convert(p.normal());

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

		DrawCuttingPlane(glm::translate<float>( centr.x -  (_planenormal.x*distance - 1),
												centr.y -  (_planenormal.y*distance - 1),
												centr.z -  (_planenormal.z*distance - 1)),
						Utility::GetRotateFromNorm(_planenormal), glm::scale<float>(5.0f, 5.0f, 5.0f));
						
		DrawTargetBody();

		/*	DRAWING METHOD:
				1.Draw all opaque objects first. -> amik nem átlátszók
				2.Sort all the transparent objects.	-> z buffer szerinti sorbarendezés , legbelsőt először
				3.Draw all the transparent objects in sorted order.	-> a sorrend szerinti rajz
		*/
		SortAlphaBlendedObjects(data);
		for (std::deque<Utility::data_t>::iterator it = SortedObjects.begin() ; it < SortedObjects.end() ; ++it)
		{
			if (cutting_mode && it->first == ActiveAtom) { continue;  }
			Draw3D(it->first, /*0.4f,*/ 1, glm::scale<float>(1.0f, 1.0f, 1.0f));
		
		}
		
	}

	glUseProgram(0);

}

void Visualization::AcceptCutting()
{
	cutting_mode = false;
	_tmpActiveAtom = 0;

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

}

void Visualization::GetResult()
{
	cutting_mode = true;
	
	app.container().cut(ActiveAtom, p);

	//data = app.cut_drawinfo();	//CSAK VÁGOTT
	MergeDataContainer(data, app.cut_drawinfo());
	//_MergeDataContainer(data, app.cut_drawinfo());
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
}

void Visualization::GetUndo()
{
	app.container().last_cut_result().undo();
	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
}

void Visualization::GetRestart()
{
	cutting_mode = false;
	NumberOfAtoms = 1;
	ActiveAtom = 0;
	_tmpActiveAtom = 0;
	app.restart();
	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
}

void Visualization::SetNewPlane()
{
	p = approx::Plane<float>(
							 { request.plane_norm.x, request.plane_norm.y, request.plane_norm.z },
		approx::Vector3<float>(request.plane_coord.x, request.plane_coord.y, request.plane_coord.z));
	distance = p.classify_point(centr);
	_planenormal = approx::convert(p.normal());
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
	m_loc_world = glGetUniformLocation(programID, "world");
	m_loc_worldIT = glGetUniformLocation(programID, "worldIT");
	eyePos = glGetUniformLocation(programID, "EyePosition");
	Lights = glGetUniformLocation(programID, "LightDirection");
	View = glGetUniformLocation(programID, "View");
	Opacity = glGetUniformLocation(programID, "opacity");
	DifCol = glGetUniformLocation(programID, "MaterialDiffuseColor");
	SpecCol = glGetUniformLocation(programID, "MaterialSpecularColor");
}

void Visualization::Draw3D(const int& which/*, float _opacity*/, const bool& backdropping, glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
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

	float _opacity;
	if (IsItActive(which)) { SetActiveAtomProperties(_opacity); }
	else				   { SetAtomProperties(_opacity); }

	glUniform1f(Opacity, _opacity);

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

	glm::mat4 mvp = m_matProj * m_matView * glm::translate<float>(15.0f,30.0f,20.0f) * glm::scale<float>(0.2f,0.2f,0.2f);
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

	glUniform1f(Opacity,  1.0f );
	SetTargetAtomProperties();

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

void Visualization::DrawCuttingPlane(glm::mat4& trans, glm::mat4& rot, glm::mat4& scal)
{
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, CuttingPlaneWireType ? GL_LINE : GL_FILL);	

	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glBindVertexArray(plane_vaoid);

	SetPlaneProperties();
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(matWorld[0][0]));
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &(matWorldIT[0][0]));

	glDrawElements(GL_TRIANGLES,		
		3 * 2 * CuttingPlaneFreq * CuttingPlaneFreq,		// hany csucspontot hasznalunk a kirajzolashoz
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
		if (cutting_mode) return;
		ActiveAtom = (ActiveAtom + 1) % NumberOfAtoms;
		centr = app.container().atoms(ActiveAtom).centroid();
		distance = p.classify_point(centr);
		_planenormal = approx::convert(p.normal());
		break;
	case SDLK_KP_MINUS:
		if (cutting_mode) return;
		ActiveAtom = (ActiveAtom - 1) % NumberOfAtoms;
		centr = app.container().atoms(ActiveAtom).centroid();
		distance = p.classify_point(centr);
		_planenormal = approx::convert(p.normal());
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
	return	(request.ta == NEGATIVE && ( ActiveAtom + _tmpActiveAtom ) == which)
		|| (request.ta == POSITIVE && (ActiveAtom + _tmpActiveAtom) + 1 == which)
		|| (request.ta == BOTH && ((ActiveAtom + _tmpActiveAtom) + 1 == which || (ActiveAtom + _tmpActiveAtom) == which));
}
void Visualization::GetInfo()
{
	std::cout << "A celtest terfogata: " << app.target().body().volume() << "\n";
	std::cout << "negativ oldali keletkezett atom terfogata: " << app.container().last_cut_result().negative()->volume() << "\n";
	std::cout << "pozitiv oldali keletkezett atom terfogata: " << app.container().last_cut_result().positive()->volume() << "\n";
}

//objektumonkénti rendezés, pontok alapján
/*TODO: Atomonként majd atomon belül indexrendezés 3-as (háromszög) csoportokban*/
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
			glm::vec3 sp = data.points[data.indicies[ObjectTriangles]];
			sps.push_back( glm::dot( c.GetEye() - sp , c.GetEye() -sp) );
		}
		auto result = *std::min_element(std::begin(sps), std::end(sps));
		SortedObjects.push_back(Utility::data_t(i, result));
	}

	std::sort(SortedObjects.begin(), SortedObjects.end(), Utility::greater_second<Utility::data_t>());
	// CSAK NEM ÁTLÁTSZO atomok eseten kéne
	/*std::deque<Utility::data_t>::iterator temp;
	Utility::data_t temp_data;
	if (!cutting_mode)
	{
		switch (request.ta)
		{
		case BOTH:
			temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom + 1; });
			temp_data = Utility::data_t(temp->first, temp->second);
			SortedObjects.erase(temp);
			SortedObjects.push_front(temp_data);

			temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom; });
			temp_data = Utility::data_t(temp->first, temp->second);
			SortedObjects.erase(temp);
			SortedObjects.push_front(temp_data);
			break;
		case POSITIVE:
			temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {
				return a.first == ActiveAtom + 1;
			});
			temp_data = Utility::data_t(temp->first, temp->second);
			SortedObjects.erase(temp);
			SortedObjects.push_front(temp_data);
			break;
		case NEGATIVE:
			temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom; });
			temp_data = Utility::data_t(temp->first, temp->second);
			SortedObjects.erase(temp);
			SortedObjects.push_front(temp_data);
			break;

		}
	}
	else {
		temp = std::find_if(SortedObjects.begin(), SortedObjects.end(), [&, this](Utility::data_t const& a) {return a.first == ActiveAtom; });
		temp_data = Utility::data_t(temp->first, temp->second);
		SortedObjects.erase(temp);
		SortedObjects.push_front(temp_data);
	}
	*/
	
}

void Visualization::SetActiveAtomProperties(float& _opacity)
{
	_opacity = 0.6f;
	glUniform3f(DifCol, 1.0f, 0.0f, 0.0f);
	glUniform3f(SpecCol, 0.6f, 0.5f, 0.2f);
}

void Visualization::SetAtomProperties(float& _opacity)
{
	_opacity = 0.3f;
	glUniform3f(DifCol, 0.0f, 0.0f, 1.0f);
	glUniform3f(SpecCol, 0.2f, 0.5f, 0.6f);
}

void Visualization::SetTargetAtomProperties()
{
	glUniform3f(DifCol, 0.0f, 1.0f, 0.0f);
	glUniform3f(SpecCol, 0.5f, 0.6f, 0.1f);
}
void Visualization::SetPlaneProperties() 
{
	glUniform3f(DifCol, 1.0f, 1.0f, 1.0f);
	glUniform3f(SpecCol, 0.5f, 0.5f, 0.5f);
}
/*Dont bother the actual atom which was cutted, just insert our datas to the end of our vectors
  need that gap size (_tmpActiveAtom)
  RANGED
  ------------------------------------------------				-----------------------------------------------
  |		|	x	|		|		|		|		|	-------->	|		|		|		|		|	x	|		|		-> _tmpActiveAtom = 3 ( 2 mert tól ig a rangedben)
  ------------------------------------------------				-----------------------------------------------
  */
void Visualization::MergeDataContainer(approx::BodyList& data, const approx::BodyList& cutresult)
{
	int CountOfPoints = (int)data.points.size();

	for (size_t i = 0; i < cutresult.points.size();++i)
	{
		data.points.push_back(cutresult.points[i]);
	}

	int LastIndexRange = data.index_ranges[data.index_ranges.size() - 1];
	
	for (size_t i = 0; i < cutresult.indicies.size();++i)
	{
		data.indicies.push_back(cutresult.indicies[i] + CountOfPoints);
	}

	for (size_t i = 1; i < cutresult.index_ranges.size();++i)
	{
		data.index_ranges.push_back(cutresult.index_ranges[i] + LastIndexRange);
	}

	_tmpActiveAtom = (int)data.index_ranges.size() - 1 - ActiveAtom - 2 ;
}

/*	Search our points which only used in the cutted atom -> collect them in a set
	Delete them in reverse mode and decrease our indexes
	Delete our unused indicies
	Insert our ponts
	Insert Indicies in 3 part , before start , new indicies, after end
	Insert Ranges in 3 part with same method
*/
void Visualization::_MergeDataContainer(approx::BodyList& data, const approx::BodyList& cutresult)
{
	int start = data.index_ranges[ActiveAtom];
	int end = data.index_ranges[ActiveAtom + 1];
	bool legvege = end == data.index_ranges[data.index_ranges.size() - 1];
	//Pontok törlése, indexek javítása
	std::set<int> DeletedPoints;

	for (int i = start + 1; i < end-1 ; ++i)
	{
		int count = 0;
		for (int j = 0; j < start; ++j)
		{
			if (data.indicies[i] == data.indicies[j]) count++;
		}
		for (size_t j = end; j < data.indicies.size(); ++j)
		{
			if (data.indicies[i] == data.indicies[j]) count++;
		}
		
		int index = data.indicies[i];
		if (!count && DeletedPoints.count(index) == 0)	//torolheto
		{
			DeletedPoints.insert(index);
		}
	}

	for (std::set<int>::reverse_iterator j = DeletedPoints.rbegin() ; j != DeletedPoints.rend() ; j++)
	{
		data.points.erase(data.points.begin() + *j);
		for (size_t k = 0; k < data.indicies.size();++k)
		{
			if ( data.indicies[k] >= *j ) { data.indicies[k]--; }
		}
	}

	//Indexek törlése
	for (int i = end - 1; i >= start ; --i)
	{
		data.indicies.erase(data.indicies.begin()+i);
	}
	//if (start == 0) { data.indicies.erase(data.indicies.begin()); }
	//if (legvege) { data.indicies.erase(data.indicies.begin()); }

	//---------------------------
	int CountOfPoints = (int)data.points.size();

	//Pontok hozzafuzese
	for (size_t i = 0; i < cutresult.points.size();++i)
	{
		data.points.push_back(cutresult.points[i]);
	}

	//Indexek hozzafuzese
	std::vector<GLushort> new_indicies(data.indicies.begin(),data.indicies.begin()+start);

	for (size_t i = 0; i < cutresult.indicies.size();++i)
	{
		new_indicies.push_back(cutresult.indicies[i] + CountOfPoints);
	}
	//már törölve vannak ezek az idnexek -> starttól
	for (size_t i = start; i < data.indicies.size();++i)
	{
		new_indicies.push_back(data.indicies[i]);
	}
	data.indicies = new_indicies;

	//Indexrange helyreallitas


	std::vector<GLushort> new_ranges(data.index_ranges.begin(), data.index_ranges.begin() + ActiveAtom);

	int LastIndexRange = new_ranges.size() == 0 ? 0 : new_ranges[new_ranges.size() - 1];
	int CutRange = cutresult.index_ranges[1] - cutresult.index_ranges[0];

	for (size_t i = 0; i < cutresult.index_ranges.size();++i)
	{
		new_ranges.push_back(cutresult.index_ranges[i] + LastIndexRange);
	}

	for (size_t i =  ActiveAtom+2; i < data.index_ranges.size();++i)
	{
		new_ranges.push_back(data.index_ranges[i] + CutRange);
	}

	data.index_ranges = new_ranges;
}