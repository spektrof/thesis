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
#include "../Utils/CameraAndLight.h"
#include "../Strategy/prior.h"
#include "../Strategy/priorityfunctions.h"
#include "../Strategy/planegetters.h"
#include "../logger/logger.h"

//----------------
#include "../UI/userinterface.hpp"
//--------------------

class Visualization
{
public:
	Visualization(void);
	~Visualization(void);

	bool Init();
	void Clean();

	void Update();
	void Render();
	float GetDistance();

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
	NumberOfAtoms : halottak is - nincs lehetõség törlésre! --> doksiba

		SHADER: olvassuk el a szöveget!!

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

	PriorityQue< approx::ConvexAtom<float>, SorterFunctions> prior 
		= PriorityQue<approx::ConvexAtom<float>, SorterFunctions>(&SorterFunctions<approx::ConvexAtom<float>>::GetVolume);
	
	//-----------------------------------------
	//Sik
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
	std::set<int> onlyActive;
	void OnlyActiveRefresh();

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
	
	bool AcceptChecker();
	//-----------------------------------------
	// Atom tulajdonsagok
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

	void LastUseChanging();

	//------------------------------------------
	// UI
	UserInterface ui;
	Request request;
	
	//Camera & light
	Camera c;
	Light l = Light(1, 100, 10, 30, 60);	//good light for us
	Light rl = Light(-1, 100, 20, 28, 88);	//real light
	Light * al;	//aktualis

	//------------------------------------------
	// Rajzolas
	int CuttingPlaneFreq;
	void MergeDataContainer(approx::BodyList&, const approx::BodyList&);

	void DrawCuttingPlane(glm::mat4&, glm::mat4&, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f));
	void Draw3D(const int&, glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(0.0f, 0.0f, 0.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void Draw2D(glm::mat4& = glm::scale<float>(1.0f, 1.0f, 1.0f), glm::mat4& = glm::translate<float>(0.0f, 0.0f, 0.0f), glm::mat4& = glm::rotate<float>(0.0f, 1.0f, 0.0f, 0.0f));
	void DrawTargetBody();
	void PartialTargetDrawRefresh();

	//--------------------------------------------
	//ID-k
	GLuint program2D_ID;
	GLuint* program3D_ID;	//amelyiket használjuk
	GLuint program3Dwhl_ID;	//Valos feny nelkuli
	GLuint program3Dl_ID;	//Valos iranyfeny

	IdsAndProp _3dIds, planeIds, targetIds;

	/* 2D ID-s */
	void Get2DDrawInfo(const std::vector<approx::PolyFace2D>&, std::vector<IdsAndProp>&, std::vector<std::vector<IdsAndProp>>&, std::vector<std::vector<IdsAndProp>>&);
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

	// ID-k shader valtozokhoz
	GLuint m_loc_mvp, m_loc_mvp2;
	GLuint world;
	GLuint view;
	GLuint proj;

	GLuint eyePos,eyePosl;
	GLuint Lights, Lightsl;
	GLuint Opacity, Opacityl;
	GLuint DifCol, DifColl;
	GLuint SpecCol, SpecColl;

	GLuint color2D;
	GLuint alpha2D;

	//----------------------------------------------

	//transzformacios matrix
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	//----------------------------------------------
	// Egyeb elemek, fuggvenyek
	std::string filename;
	bool logger;
	bool IsTargetDrawEnabled;
	bool partialTarget;

	float targetDistance;
	float CalculateDistance();

	void CleanIdBufferForReuse(const IdsAndProp);
	void Release2DIds();
	void Add2DShaders();
	void Add3DShaders();
	void Add3DlShaders();
	void AddShaderUniformLocations();
	void SetUniforms(const glm::mat4& matWorld);
};