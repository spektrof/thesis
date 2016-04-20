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
	void KeyboardUp(SDL_KeyboardEvent&);
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
			Engine class , nem biztos hogy mindent le kell t�rolni csak a legsz�ks�gessebeket!
			Plane class, az �sszes plane tulajdons�g (centroidt�l vett t�v stb), s�kgener�l� fv pointer stb
			Atom tul, priorit�s fv, �s minden m�s
			Visual : active atom, minden ami a rajzol�shoz kell, idk draw fv ek, stb
			+ doksi
		Szerda:
			ui doksi + ui �t�r�s
				UI: sima pointereket �t�rni unique_ptr re!
			n+1) Projection matrix: perspective vs ortho -> camera optimizing
		Cs�t�rt�k
			-
		P�ntek:
			k�pek + �jraolvas�s + pontos�t�s
			elk�ldeni G�bornak
		Szombat:
			Kommentel�s
			Leakek keres�se: VMI szemetel - MI????
				- minden pointert t�r�lj�nk majd hozzuk l�tre ami olyan (oszt�lyon bel�l is - destruktorok)
		Vas�rnap:
			Doksi olvas�s + friss�t�s
			
		s�kill

		BUG:

		Apr� bugok friss�t�s gyan�nt:
		
		Apr� kis TODOk:
			draw3d, opacity be a setpropba
		
	*//*	Blender : doksi*/
	/*			+ WorldIT : normal �gy tarja meg mag�t -> mivel ezt sz�molom nem kell
				+ World : esetleg mikor f�nyt sz�molok akkor sz�m�t
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

	void SetActiveAtomProperties(float&);
	void SetAtomProperties(float&);
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
	std::string filename;
	bool logger = false;

	void CleanIdBufferForReuse(const IdsAndProp);
	void Release2DIds();
	void Add2DShaders();
	void Add3DShaders();
	void AddShaderUniformLocations();
};
