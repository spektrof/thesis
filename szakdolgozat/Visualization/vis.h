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
		-El�fordulhat hogy eleg nagy a metszetterfogat 
			de a fourier egyutthato mar nem eleg nagy -> halott atom, amirol nem tudunk
		-A s�kv�laszt� f�ggv�ny jelenleg itt van defini�lva - 
				�trakjam a planegetters be �s akkor ott egy saj�t fv-�re  hivatkozik?
				
				Settert + Gettert kell r��rni �gy , nekem tetszik �gy is
		-Kamera + f�ny m�dos�t�sok bemutat�sa
		-Ha elfogytak az elo atomok:
			ActiveAtom = -1
			�s lev�dem mindenhol liveatoms.size()!=0 felt�tellel a dolgokat
				a s�klek�r�sn�l nem j� szerintem az ha minden strat�gia ezzel kezd�dik (Activeatom!=-1) �gy k�v�lr�l int�zn�m el
				-> Lehessen v�ltoztatni strat�gi�t (sok if ebben az esetben) vagy ne (j�jj�n hiba�zenet - vagy az nem kell) -> UI helyre�ll�t� fv-kell -> nagy switch �gy els� �tgondol�sra UI ont bel�l
				DE ez nem kell akkor sem ha minden s�klek�r�sn�l vizsg�lunk
				TEH�T lehets�ges opci�k:
					- Minden s�kv�laszt�s IF(ActiveAtom==-1) return default; al kezd�dik
					- Lehet strat�gi�t v�ltoztatni ( mert m�rt ne lehetne ), csak �pp nem t�rt�nik semmi �s mindenhol liveatoms size ra vizsg�lunk vis.cpp ben
							-> probl�ma hogy manu�lis m�dban nem fog v�ltozni semmi, mert a sikrajzolasunk nem engedi meg ezt(nem tudunk centroidot szamolni)
		-Restart j� �gy - vagy a s�kv�laszt�s se v�ltozzon - vagy a rendez�s is v�ltozzon ( ez lehet gubanc lesz - mert a v�ltoztat�s �rt�k eventet okoz , de meglehet nezni)
					- mivel manu�lis a default �rt�k s�kv�laszt�sn�l - de akkor �gy akkor legn. t�rfogat lesz a default - nem tudom mi�rt vettem ki (lehet probl�ma volt)
		-lastuse os bug - pointer egyszer megy fol, + rossz volt
		-leak
		-�j gomb : t
		-ablakmodositas + hibauzenet ui bug?
		-UI unique ptr

		Kedd: 
		Szerda:
		Cs�t�rt�k
		P�ntek:
		Szombat:
		Vas�rnap:
				
		LEAK:
		/*
			import�l�s ->engineinit() : pl torus test v�ltakoz�sa eset�n testn�l nem ugrik vissza a mem�ria
			1000 v�g�s ut�ni restart: +0,5 mb
			sok strategy v�lt�s = +0,1 mb
		*/
	/*
		UI: generated files !

		BUG:

		Apr� bugok friss�t�s gyan�nt:
		
		Apr� kis TODOk:
			GOMB: ortho es perspektiv nezeti valtasra (3D ben)

	*//*	Blender : doksi*/
	/*			+ WorldIT : normal �gy tarja meg mag�t -> mivel ezt sz�molom nem kell
				+ World : esetleg mikor f�nyt sz�molok akkor sz�m�t
				+ f�ny is mozog ha kamer�val mozgunk!
				http://www.kickjs.org/example/shader_editor/shader_editor.html#
				http://on-demand.gputechconf.com/gtc/2014/presentations/S4385-order-independent-transparency-opengl.pdf
	*/
	//-----------------------------------------
	// ENGINE
	bool EngineInit();
	 
	approx::Approximator<float> app;
	approx::BodyList data;

	PriorityQue< approx::ConvexAtom<float>, SorterFunctions> prior 
		= PriorityQue<approx::ConvexAtom<float>, SorterFunctions>(&SorterFunctions<approx::ConvexAtom<float>>::GetVolume, &lastUse);
	
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
	Light l;

	//------------------------------------------
	// Rajzolas
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

	// ID-k shader valtozokhoz
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

	//transzformacios matrix
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	//----------------------------------------------
	// Egyeb elemek, fuggvenyek
	std::string filename;
	bool logger;
	bool IsTargetDrawEnabled;

	void CleanIdBufferForReuse(const IdsAndProp);
	void Release2DIds();
	void Add2DShaders();
	void Add3DShaders();
	void AddShaderUniformLocations();
};
