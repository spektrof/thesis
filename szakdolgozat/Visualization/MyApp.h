#pragma once

#include <GL/glew.h>

#include <vector>
#include <set>

#include <SDL.h>
#include <SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <glm/gtc/type_ptr.hpp> //valueptr

//----------------
#include "../Utils/ObjectCreator.h"
#include "../Utils/Utility.h"
#include "../Utils/CameraAndLight.h"
#include "../prior.h"
#include "../priorityfunctions.h"
#include "../planegetters.h"
#include "../logger/logger.h"

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
		Blender : doksi

		Kérdések:
			- 2D , lsd 1.a)
					- a pontok lekérése: mikor kérhetem? (vágás után, accept után) ; most createljek mindent minden futásban vagy akkor mergeljem össze?
																VAGY indexeket teszek egy vectorba és ezek futnak le ciklusban
			- síkillesztés (x,y,f(x,y)) ? 2D hez ?
			- Fourier numerikus hiba -> epszilon? : megvalósításom?
			- véletlen felülethez lekérés
			- planegetters megvalósítások

		2D//LINESTRIP - 2 színû, belsõ külsõ
		// + az atom átláts stb tri fan
		1.a) 2D :		
				Probléma:
					Geometry Shader - meg kell adni a bemenõ típust ! https://www.opengl.org/wiki/Geometry_Shader
							Triangle VS Point?
					Solution:
						2 ProgramID
						1. : 2D hez |
										= > Külön shaderek (2D_vertex, 3D_vertex, geometry, 3D_fragment, 3D_vertex)
						2. : 3D hez |
						
						RENDERNÉL:
							Camera check után mennek a glUse ok
			Test projekt: c:\Users\...\Documents\Visual Studio 2015\Projects\2dtesting\2dtesting\

		1.b) Adj matrix, lsd: planegetters.h

		2.Tovabbi strategyk
	
		4.optimizing: pl lehetne mindig Plane típust visszaadni planegettersben?
		n. Headerben rendezni a dolgokat

		n+1) Projection matrix: perspective vs ortho -> camera optimizing

		Apró bugok frissítés gyanánt:
			- Fourier mezõ inaktív vágásnál!
			- 0,0,0 normálisú sík WARNING

	*/
	PriorityQue< approx::ConvexAtom<float>, SorterFunctions> prior =
		PriorityQue<approx::ConvexAtom<float>, SorterFunctions>(&SorterFunctions<approx::ConvexAtom<float>>::GetVolume);

	// ENGINE
	bool EngineInit();
	void _MergeDataContainer(approx::BodyList&,const approx::BodyList&);
	 
	approx::Approximator<float> app;
	approx::BodyList data;
	approx::BodyList targetdata;
	std::vector<approx::PolyFace2D> _2Ddata;

	approx::Plane<float> p = approx::Plane<float>({ 0,0,1 }, approx::Vector3<float>(0.0f,0.0f,26.0f));  //vagosik

	approx::Vector3<float> centr;
	float distance;
	glm::vec3 _planenormal;

	void RefreshPlaneData(const Utility::PlaneResult&);
	PlaneGetterFunctions<approx::Approximation<float>>* PlaneCalculator = new PlaneGetterFunctions<approx::Approximation<float>>();

	typedef Utility::PlaneResult(PlaneGetterFunctions<approx::Approximation<float>>::*PlaneGetter)() const;
	PlaneGetter PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::Manual);
	//---------------------------------------
	//DISPLAY
	std::set<int>* display = new std::set<int>();
	std::set<int> liveAtoms;
	std::set<int> relevantAtoms;

	float fourier;
	void CalculateDisplayVectorsByFourier(const TypeOfAccept& ta);

	//----------------------------------------
	// EVENTS
	void GetUndo();
	void GetResult();
	void AcceptCutting();
	void GetRestart();
	void SetNewPlane();
	void SetNewStrategy();
	void SetNewCuttingMode();
	void SetNewDisplayMode();

	bool cutting_mode = false;

	void CutChecker();

	//-----------------------------------------
	// Atom properties
	void GetInfo();
	bool IsItActive(const int&);

	std::vector <int> lastUse;
	std::vector<int> priorQue;
	int ActiveAtom, ActiveIndex;
	bool transparency = true;
	GLuint NumberOfAtoms = 1;

	/*std::vector<Utility::data_t> SortedObjects;
	void SortAlphaBlendedObjects(approx::BodyList&);*/

	void GetPriorResult();

	void SetActiveAtomProperties(float&);
	void SetAtomProperties(float&);
	void SetTargetAtomProperties();
	void SetPlaneProperties();

	void NextAtom();
	void PrevAtom();

	//------------------------------------------
	// UI
	UserInterface ui;
	Request request;
	
	//Camera & light
	Camera c;
	Light l;

	//------------------------------------------
	// DRAWING
	int CuttingPlaneFreq = 12;
	void DrawCuttingPlane(glm::mat4&, glm::mat4&, glm::mat4&);
	void Draw3D(const int&, /*float,*/ const bool& = true, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
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
	GLuint m_loc_world;
	GLuint m_loc_worldIT;

	GLuint eyePos;
	GLuint Lights;
	GLuint View;
	GLuint Opacity;
	GLuint DifCol;
	GLuint SpecCol;

	//----------------------------------------------

	//transformation matrix
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	//----------------------------------------------
	// Structure
	bool logger = false;

	void AddShaders();
	void AddShaderUniformLocations();
};
