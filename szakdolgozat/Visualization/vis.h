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
#include "../Strategy/prior.h"
#include "../Strategy/priorityfunctions.h"
#include "../Strategy/planegetters.h"
#include "../logger/logger.h"

//----------------
#include "../UI/userinterface.hpp"
#include "../UI/request.h"
//--------------------
#include "../Engine/approximator.h"

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
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);

	void Resize(int, int);

	struct IdsAndProp
	{
		int count;
		glm::vec3 eye;
		GLuint VaoId;
		GLuint VboId;
		GLuint IndexId;
		IdsAndProp(const int c = 0, const glm::vec3& e = glm::vec3(0, 0, 0), const GLuint vao = 0, const GLuint vbo = 0, const GLuint i = 0) : VaoId(vao), VboId(vbo), IndexId(i), count(c), eye(e) {}
	};
	
protected:
	/*	TODO list*/
	/*
		Kedd: 
		Szerda:
		Csütörtök
		Péntek:
			képek + újraolvasás + pontosítás
			elküldeni Gábornak
		Szombat:
			Kommentelés
		Vasárnap:
			Doksi olvasás + frissítés
			
		
		LastUse - pointert állítunk, pl konstruktorban - utána már csak frissítés legyen
		Szomszédsági mátrixot nem kell tárolni, csak 1x feltölteni konstruktorban és kész
		
		LEAK:
		/*
			importálás ->engineinit() : pl torus test váltakozása esetén testnél nem ugrik vissza a memória
			1000 vágás utáni restart: +0,5 mb
			sok strategy váltás = +0,1 mb
		*/
	/*
		BUG:

		Apró bugok frissítés gyanánt:
		
		Apró kis TODOk:
			GOMB: ortho es perspektiv nezeti valtasra (3D ben)
			szomszédsági mátrixot elég a planegettersben tárolni, nem pointerrel!

	*//*	Blender : doksi*/
	/*			+ WorldIT : normal így tarja meg magát -> mivel ezt számolom nem kell
				+ World : esetleg mikor fényt számolok akkor számít
				+ fény is mozog ha kamerával mozgunk!
				http://www.kickjs.org/example/shader_editor/shader_editor.html#
				http://on-demand.gputechconf.com/gtc/2014/presentations/S4385-order-independent-transparency-opengl.pdf
	*/
	//-----------------------------------------
	// ENGINE
	bool EngineInit();
	 
	approx::Approximator<float> app;
	approx::BodyList data;
	approx::BodyList targetdata;
	std::vector<approx::PolyFace2D> _2Ddata;

	std::vector< std::vector<int> > adj_mtx;

	PriorityQue< approx::ConvexAtom<float>, SorterFunctions> prior 
		= PriorityQue<approx::ConvexAtom<float>, SorterFunctions>(&SorterFunctions<approx::ConvexAtom<float>>::GetVolume);
	
	//-----------------------------------------
	//PLANE
	approx::Plane<float> p = approx::Plane<float>({ 1,0,0 }, approx::Vector3<float>(0.0f,0.0f,0.0f));  //vagosik

	approx::Vector3<float> centr;
	float distance;
	glm::vec3 _planenormal;

	void RefreshPlaneData(const Utility::PlaneResult&);
	PlaneGetterFunctions<approx::Approximation<float>>* PlaneCalculator;

	typedef Utility::PlaneResult(PlaneGetterFunctions<approx::Approximation<float>>::*PlaneGetter)() const;
	PlaneGetter PlaneFunction;

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
	void ImportNewTarget();
	void NextAtom();
	void PrevAtom();
	
	void CutChecker();

	//-----------------------------------------
	// Atom properties
	bool IsItActive(const int&);

	std::vector <int> lastUse;
	std::vector<int> priorQue;
	int ActiveAtom, ActiveIndex;
	GLuint NumberOfAtoms;

	void GetPriorResult();

	void SetActiveAtomProperties();
	void SetAtomProperties();
	void SetTargetAtomProperties();
	void SetPlaneProperties();

	void LastUseChanging(const TypeOfAccept&);



	//------------------------------------------
	// UI
	UserInterface ui;
	Request request;
	
	//Camera & light
	Camera c;
	Light l;

	//------------------------------------------
	// DRAWING
	int CuttingPlaneFreq;
	void MergeDataContainer(approx::BodyList&, const approx::BodyList&);

	void DrawCuttingPlane(glm::mat4&, glm::mat4&, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f));
	void Draw3D(const int&, const bool& = true, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(0.0f, 0.0f, 0.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void Draw2D(glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(0.0f, 0.0f, 0.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void DrawTargetBody();

	//--------------------------------------------
	//IDs
	GLuint program2D_ID;
	GLuint program3D_ID;

	IdsAndProp _3dIds, planeIds, targetIds;

	/* 2D ID-s */
	void Get2DDrawInfo();
	int Active2DIndex;

	std::vector<IdsAndProp>* _2DTri;
	std::vector<std::vector<IdsAndProp>>* _2DLine1;
	std::vector<std::vector<IdsAndProp>>* _2DLine2;

	std::vector<IdsAndProp> _2D_TriIds_N;
	std::vector<std::vector<IdsAndProp>> _2D_Line1Ids_N;
	std::vector<std::vector<IdsAndProp>> _2D_Line2Ids_N;

	std::vector<IdsAndProp> _2D_TriIds_P;
	std::vector<std::vector<IdsAndProp>> _2D_Line1Ids_P;
	std::vector<std::vector<IdsAndProp>> _2D_Line2Ids_P;

	// IDs of shader variables
	GLuint m_loc_mvp;
	GLuint m_loc_mvp2;
	GLuint world;

	GLuint eyePos;
	GLuint Lights;
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
	std::string filename;
	bool logger = false;

	void CleanIdBufferForReuse(const IdsAndProp);
	void Release2DIds();
	void Add2DShaders();
	void Add3DShaders();
	void AddShaderUniformLocations();
};
