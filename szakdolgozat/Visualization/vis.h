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
		-Elõfordulhat hogy eleg nagy a metszetterfogat 
			de a fourier egyutthato mar nem eleg nagy -> halott atom, amirol nem tudunk
		-A síkválasztó függvény jelenleg itt van definiálva - 
				átrakjam a planegetters be és akkor ott egy saját fv-ére  hivatkozik?
				
				Settert + Gettert kell ráírni így , nekem tetszik így is
		-Kamera + fény módosítások bemutatása
		-Ha elfogytak az elo atomok:
			ActiveAtom = -1
			és levédem mindenhol liveatoms.size()!=0 feltétellel a dolgokat
				a síklekérésnél nem jó szerintem az ha minden stratégia ezzel kezdõdik (Activeatom!=-1) így kívülrõl intézném el
				-> Lehessen változtatni stratégiát (sok if ebben az esetben) vagy ne (jöjjön hibaüzenet - vagy az nem kell) -> UI helyreállító fv-kell -> nagy switch így elsõ átgondolásra UI ont belül
				DE ez nem kell akkor sem ha minden síklekérésnél vizsgálunk
				TEHÁT lehetséges opciók:
					- Minden síkválasztás IF(ActiveAtom==-1) return default; al kezdõdik
					- Lehet stratégiát változtatni ( mert mért ne lehetne ), csak épp nem történik semmi és mindenhol liveatoms size ra vizsgálunk vis.cpp ben
							-> probléma hogy manuális módban nem fog változni semmi, mert a sikrajzolasunk nem engedi meg ezt(nem tudunk centroidot szamolni)
		-Restart jó így - vagy a síkválasztás se változzon - vagy a rendezés is változzon ( ez lehet gubanc lesz - mert a változtatás érték eventet okoz , de meglehet nezni)
					- mivel manuális a default érték síkválasztásnál - de akkor így akkor legn. térfogat lesz a default - nem tudom miért vettem ki (lehet probléma volt)
		-lastuse os bug - pointer egyszer megy fol, + rossz volt
		-leak
		-új gomb : t
		-ablakmodositas + hibauzenet ui bug?
		-UI unique ptr

		Kedd: 
		Szerda:
		Csütörtök
		Péntek:
		Szombat:
		Vasárnap:
				
		LEAK:
		/*
			importálás ->engineinit() : pl torus test váltakozása esetén testnél nem ugrik vissza a memória
			1000 vágás utáni restart: +0,5 mb
			sok strategy váltás = +0,1 mb
		*/
	/*
		UI: generated files !

		BUG:

		Apró bugok frissítés gyanánt:
		
		Apró kis TODOk:
			GOMB: ortho es perspektiv nezeti valtasra (3D ben)

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
