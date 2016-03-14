#pragma once

#include <GL/glew.h>

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

//----------------

#include "../UI/userinterface.hpp"
#include "../UI/request.h"

#include "../szakdoga/approximator.h"

class CMyApp
{
	bool EngineInit();

public:
	CMyApp(void);
	~CMyApp(void);

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
		SHADER - opacity, egymásban lévõ testeket lássunk
		Klikkel atom vagy jó így??
	*/
	approx::Approximator<float> app;
	approx::BodyList data;
	approx::BodyList targetdata;

	approx::Plane<float> p = approx::Plane<float>({ 1,0,0 }, 15.0f);  //vagosik

	void GetUndo();
	void GetResult();
	void AcceptCutting();
	void GetRestart();

	bool IsItActive(const int&);

	UserInterface ui;
	Request request;

	//TODO: Mikor rajzolom a vágósíkot a testhez a scale skálázza a koordinátákat mind3 tengely mentén így az eltolást ennek megfelelõen
	//		x-1 el el kell tolni, ahol x a test skálája az egység kockához képest    - ez nem okoz gondot ha én mondok síkot az enginek
	//		DE ott is figyelni kell erre !!!!!!!! (fordított eset)
	glm::mat4 GetTranslateFromCoord(glm::vec3 vec);
	glm::mat4 GetRotateFromNorm(glm::vec3 vec);
	
	void CreateCuttingPlane();
	void Create2DObject(const std::vector<glm::vec2>&);
	void Create3DObject(const std::vector<glm::vec3>&, const std::vector<unsigned short>&);
	void Ideiglenes3DKocka();
	void GetInfo();

	void DrawCuttingPlane(glm::mat4&, glm::mat4&, glm::mat4&);
	void Draw3D(const int&, const float&, const bool& = true, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void Draw2D(const int&, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));


	GLuint programID;

	GLuint _3DvaoID,_2DvaoID, plane_vaoid;
	GLuint _3DvboID,_2DvboID, plane_vboid;
	GLuint _3Dindex,plane_index;
	GLuint ibID;
	GLuint CountsOfIndexes; // ideiglenes kockahoz

	//transformation matrix
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	// IDs of shader variables
	GLuint m_loc_world;
	GLuint m_loc_worldIT;
	GLuint m_loc_mvp;

	GLuint eyePos;
	GLuint Light;
	GLuint View;
	GLuint Opacity;
	int ActiveAtom;

	struct Vertex
	{
		glm::vec3 p;
		//glm::vec3 n;
		//glm::vec3 c; //feleslegessé válik szerintem ha megadunk mindig anyagszínt
	};

	void AddShaders();
	void AddShaderUniformLocations();
	void BindingBufferData(GLuint*, GLuint*, const std::vector<Vertex>);
	void BindingBufferIndicies(GLuint*, const std::vector<GLushort>);

	bool is_left_pressed = false;	// camera moving
	bool transparency = true;
	bool cutting_mode = false;

	GLuint NumberOfAtoms = 1;

	//Kamera és fényes dolgok
	enum ViewPos{
		_3D,_2D
	};
	ViewPos ViewPoint;

	glm::vec3 ToDescartes(float u, float v);

	float u = 4.0f, v = 2.0f;

	glm::vec3 fw = ToDescartes(4, 2);
	glm::vec3 eye = glm::vec3(65, 50, 90);
	//glm::vec3 eye = glm::vec3(50, 35, 50);
	glm::vec3 up = glm::vec3(0, 1, 0);

	glm::vec3 at = eye + fw;
	glm::vec3 lw = ToDescartes(-u, v); //fényegység
	glm::vec3 FenyIrany = glm::vec3(1, -1, 1) + lw;

};
