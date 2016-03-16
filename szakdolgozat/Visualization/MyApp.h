#pragma once

#include <GL/glew.h>

#include <vector>
#include <deque>
#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <glm/gtc/type_ptr.hpp> //valueptr

//----------------
#include "../ObjectCreator.h"
#include "../Utility.h"
#include "../CameraAndLight.h"

//----------------
#include "../UI/userinterface.hpp"
#include "../UI/request.h"
//--------------------
#include "../szakdoga/approximator.h"

class Visualization
{
public:
	Visualization(void);
	~Visualization(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);

	void Resize(int, int);

protected:
	/*	TODO list
		 Sík BUG - y,z normálisok nem jól mennek a síknál
		 HACK: mikor elfogadok egy vágást a sorter (blendhez) nem tud keresni active + 1 et -> error
	*/

	// ENGINE
	bool EngineInit();

	approx::Approximator<float> app;
	approx::BodyList data;
	approx::BodyList targetdata;

	//MOST : Kell vmi default sík
	approx::Plane<float> p = approx::Plane<float>({ 1,1,1 }, approx::Vector3<float>(15.0f,30.0f,30.0f));  //vagosik

	//----------------------------------------
	// EVENTS
	void GetUndo();
	void GetResult();
	void AcceptCutting();
	void GetRestart();
	void SetNewPlane();

	bool cutting_mode = false;

	//-----------------------------------------
	// Atom properties
	void GetInfo();
	bool IsItActive(const int&);

	int ActiveAtom;
	bool transparency = true;
	GLuint NumberOfAtoms = 1;

	std::deque<Utility::data_t> SortedObjects;
	void SortAlphaBlendedObjects(approx::BodyList&);

	//------------------------------------------
	// UI
	UserInterface ui;
	Request request;
	
	//Camera & light
	Camera c;
	Light l;

	//------------------------------------------
	// DRAWING
	void DrawCuttingPlane(glm::mat4&, glm::mat4&, glm::mat4&);
	void Draw3D(const int&, const float&, const bool& = true, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void Draw2D(const int&, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void DrawTargetBody();

	//--------------------------------------------
	// IDs
	GLuint programID;

	GLuint _3DvaoID, _2DvaoID, plane_vaoid;
	GLuint _target_vaoID, _target_vboID, _target_indexID;
	GLuint _3DvboID,_2DvboID, plane_vboid;
	GLuint _3Dindex,plane_index;

	// IDs of shader variables
	GLuint m_loc_mvp;

	GLuint eyePos;
	GLuint Lights;
	GLuint View;
	GLuint Opacity;

	//----------------------------------------------

	//transformation matrix
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	//----------------------------------------------
	// Structure
	void AddShaders();
	void AddShaderUniformLocations();
};
