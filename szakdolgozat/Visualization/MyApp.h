#pragma once

/*	Keszitette: Lukacs Peter

	A megjelenitest felugyelo osztaly. (OpenGL)
	Ezen kivul feldolgozza a UI -rol jovo kereseket. (Qt)
*/

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

	struct IdsAndVertC
	{
		int count;
		glm::vec3 eye;
		GLuint VaoId;
		GLuint VboId;
		GLuint IndexId;
		IdsAndVertC(const int c = 0, const glm::vec3& e = glm::vec3(0, 0, 0), const GLuint vao = 0, const GLuint vbo = 0, const GLuint i = 0) : VaoId(vao), VboId(vbo), IndexId(i), count(c), eye(e) {}
	};
	
protected:
	/*	TODO list*/
	/*
		n+1) Projection matrix: perspective vs ortho -> camera optimizing
		DeleteBuffersek hívása
		Vissza az MVP kell!!! (síkot tologatom)

		Apró bugok frissítés gyanánt:
		Vagj 10 et veletlennel es a 10.nek a pozitiv vetuletei nem kozepre kerulnek, miert?
				Test: 9 et vagok siman es aztan 1 et debuggal - majd szamolok hanyadik alakzat majd megnezem hogy az e a kamera amit szamoltam stb
		Apró kis TODOk:
	
	*//*	Blender : doksi*/
	/*			+ WorldIT : normal így tarja meg magát -> mivel ezt számolom nem kell
				+ World : esetleg mikor fényt számolok akkor számít
				http://www.kickjs.org/example/shader_editor/shader_editor.html#
				http://on-demand.gputechconf.com/gtc/2014/presentations/S4385-order-independent-transparency-opengl.pdf
	*/
	GLuint vaoID, vboID;
	GLuint LineVaoID, LineVboID;
	GLuint LinePointsVaoID, LinePointsVboID;

	void CreateXAndY();
	void DrawXAndY();
	void CreateXAndYPoints();
	void DrawXAndYPoints();
	//-----------------------------------------
	// ENGINE
	bool EngineInit();
	 
	approx::Approximator<float> app;
	approx::BodyList data;
	approx::BodyList targetdata;
	std::vector<approx::PolyFace2D> _2Ddata;

	std::vector< std::vector<int> > adj_mtx;

	PriorityQue< approx::ConvexAtom<float>, SorterFunctions> prior =
		PriorityQue<approx::ConvexAtom<float>, SorterFunctions>(&SorterFunctions<approx::ConvexAtom<float>>::GetVolume);
	
	//-----------------------------------------
	//PLANE
	approx::Plane<float> p = approx::Plane<float>({ 1,0,0 }, approx::Vector3<float>(0.0f,0.0f,0.0f));  //vagosik

	approx::Vector3<float> centr;
	float distance;
	glm::vec3 _planenormal;

	void RefreshPlaneData(const Utility::PlaneResult&);
	PlaneGetterFunctions<approx::Approximation<float>>* PlaneCalculator;

	typedef Utility::PlaneResult(PlaneGetterFunctions<approx::Approximation<float>>::*PlaneGetter)() const;
	PlaneGetter PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::Manual);

	//---------------------------------------
	//DISPLAY
	std::set<int>* display;
	std::set<int> liveAtoms;
	std::set<int> relevantAtoms;

	void CalculateDisplayVectorsByFourier();

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

	void CutChecker();

	//-----------------------------------------
	// Atom properties
	bool IsItActive(const int&);

	std::vector <int> lastUse;
	std::vector<int> priorQue;
	int ActiveAtom, ActiveIndex;
	GLuint NumberOfAtoms = 1;

	void GetPriorResult();

	void SetActiveAtomProperties(float&);
	void SetAtomProperties(float&);
	void SetTargetAtomProperties();
	void SetPlaneProperties();

	void LastUseChanging(const TypeOfAccept&);

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
	void _MergeDataContainer(approx::BodyList&, const approx::BodyList&);

	void DrawCuttingPlane(glm::mat4&, glm::mat4&, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f));
	void Draw3D(const int&, const bool& = true, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void Draw2D(glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void DrawTargetBody();

	//--------------------------------------------
	//IDs
	GLuint program2D_ID;
	GLuint program3D_ID;

	IdsAndVertC _3dIds, planeIds, targetIds;

	/* 2D ID-s */
	void Get2DDrawInfo();
	int Active2DIndex;

	std::vector<IdsAndVertC>* _2DTri = new std::vector<IdsAndVertC>();
	std::vector<std::vector<IdsAndVertC>>* _2DLine1 = new std::vector<std::vector<IdsAndVertC>>();
	std::vector<std::vector<IdsAndVertC>>* _2DLine2 = new std::vector<std::vector<IdsAndVertC>>();

	std::vector<IdsAndVertC> _2D_TriIds_N;
	std::vector<std::vector<IdsAndVertC>> _2D_Line1Ids_N;
	std::vector<std::vector<IdsAndVertC>> _2D_Line2Ids_N;

	std::vector<IdsAndVertC> _2D_TriIds_P;
	std::vector<std::vector<IdsAndVertC>> _2D_Line1Ids_P;
	std::vector<std::vector<IdsAndVertC>> _2D_Line2Ids_P;

	// IDs of shader variables
	GLuint m_loc_mvp;
	GLuint m_loc_mvp2;
	GLuint world;

	GLuint eyePos;
	GLuint Lights;
	GLuint View;
	GLuint Opacity;
	GLuint DifCol;
	GLuint SpecCol;

	GLuint color2D;
	GLuint alpha2D;


	//----------------------------------------------

	//transformation matrix
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	//----------------------------------------------
	// Structure
	bool logger = false;

	void Add2DShaders();
	void Add3DShaders();
	void AddShaderUniformLocations();
};
