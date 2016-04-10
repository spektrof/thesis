#include "MyApp.h"
#include "../Utils/GLUtils.hpp"

#include <GL/GLU.h>

#define CuttingPlaneWireType 1
#define _3DWireType 0
#define FOURIERCOEFFICIENT 0.5
#define EPSILONFOURIER 0.0001f

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
	
	program2D_ID = 0;
	program3D_ID = 0;

	ActiveAtom = 0;
	ActiveIndex = 0;
}

Visualization::~Visualization(void)
{
	Clean();
}

/* Inicializálás:
	Approximáló Init
	Törlés
	Hátlapeldobás
	Vágósík létrehozása
	Shaderek bekötése 2D és 3D hez
	Shaderekhez a uniform változók bekötése
	*/
bool Visualization::Init()
{
	if (!EngineInit()) {	LOG("ERROR during engine init!\n"); return false;  }

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	ObjectCreator::CreateCuttingPlane(plane_vaoid,plane_vboid,plane_index,50, CuttingPlaneFreq);	// (x,y síkban fekvő , (0,0,1) normálisú négyzet)
	 
	Add2DShaders();
	Add3DShaders();
	AddShaderUniformLocations();

	LOG("---------------- INIT DONE ------------------\n"
			"---------------------------------------------\n");

	return true;
}

/* Approximációhoz + alap megjelenítéshez szükséges opjektumok lekérése */
bool Visualization::EngineInit() 
{
	if (!app.set_target("Targets/test.obj", 10.0f, 0.0f, 20.0f)) {
		LOG("HIBA A FAJL BETOLTESENEL!\n");
		return false;
	}
	
	adj_mtx = Utility::GetAdjacencyMatrix(&(app.target().face_container()));
	PlaneCalculator = new PlaneGetterFunctions<approx::Approximation<float>>(&app.container(),&adj_mtx);

	data = app.atom_drawinfo();
	targetdata = app.target_drawinfo();

	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);
	ObjectCreator::Create3DObject(targetdata.points, targetdata.indicies,_target_vaoID,_target_vboID, _target_indexID);
	
	lastUse.push_back(0);
	prior.SetLastUse(&lastUse);

	CalculateDisplayVectorsByFourier(NEGATIVE);
	display = &liveAtoms;

	GetPriorResult();
	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

	/*------------------------------------*/
	//std::vector< std::vector<int> > adj_mtx = Utility::GetAdjacencyMatrix2(&(app.target().face_container()));		//lassu

	return true;
}

/* Renderelo : 2D vagy 3D ben ábrázol 
		3D-ben fogad requesteket!
*/
void Visualization::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (c.Is2DView())	// true when _2D
	{
		glUseProgram(program2D_ID);
		
		switch (request.eventtype)
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
			case NEWSTRATEGY:
				SetNewStrategy();
				break;
			case NEWCUTTINGMODE:
				SetNewCuttingMode();
				break;
			case NEWDISPLAY:
				SetNewDisplayMode();
				break;
			case RESTART:
				c.SwitchCameraView();
				GetRestart();
				break;
			case NEXTATOM:
				NextAtom();
				break;
			case PREVATOM:
				PrevAtom();
				break;
			case RECALCULATING:
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				break;
			case MORESTEPS:
				for (int i = 0; i < request.CountsOfCutting; ++i) { GetResult(); }
				break;
			case EXPORT:
				break;
			default:
				break;
		}

		Draw2D();
	}
	else // _3D
	{
		glUseProgram(program3D_ID);

		glUniform3fv(eyePos, 1, glm::value_ptr(c.GetEye()));
		glUniform3fv(Lights, 1, glm::value_ptr(l.GetLightDir()));
		glUniform1i(View, c.GetView());

		switch (request.eventtype)
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
			case NEWSTRATEGY:
				SetNewStrategy();
				break;
			case NEWCUTTINGMODE:
				SetNewCuttingMode();
				break;
			case NEWDISPLAY:
				SetNewDisplayMode();
				break;
			case RESTART:
				GetRestart();
				break;
			case NEXTATOM:
				NextAtom();
				break;
			case PREVATOM:
				PrevAtom();
				break;
			case RECALCULATING:
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				break;
			case MORESTEPS:
				for (int i = 0; i < request.CountsOfCutting; ++i)	{	GetResult();	}
				break;
			case EXPORT:
				break;
			default:
				break;
		}


		DrawCuttingPlane(Utility::GetTranslate(centr,_planenormal,distance),Utility::GetRotateFromNorm(_planenormal));
						
		DrawTargetBody();

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::mat4 matWorld = glm::mat4(1);
		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

		glm::mat4 mvp = m_matProj * m_matView * matWorld;

		glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
		glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(matWorld[0][0]));
		glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &(matWorldIT[0][0]));
	

		for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it)
		{
			Draw3D(*it ,1);
		}

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	glUseProgram(0);

}

/* Amikor elfogadunk egy vágást ez a függvény fut le */
void Visualization::AcceptCutting()
{
	/* Folyamat:
		Elfogadjuk az adott vágást, ha sikeres volt
		Lekérjük az új rajzolási információkat
		Létrehozzuk az új testet
		Aktualizáljuk a legrégebben használt vektorunkat
		Aktualizáljuk fourier egy. alapján az élő és releváns atomjainkat
		Lekérjük az aktuális prior sorrendet + vágósíkot
		Log			*/
	switch (request.type)
	{ 
		case BOTH:
			if (!app.container().last_cut_result().choose_both()) { ui.ErrorShow("ERROR BOTH\nThe cut is dopped!"); return; }

			NumberOfAtoms++;
			break;
		case POSITIVE:
			if (!app.container().last_cut_result().choose_positive()) { ui.ErrorShow("ERROR POSITIVE\nThe cut is dopped!"); return; }

			break;
		case NEGATIVE:
			if (!app.container().last_cut_result().choose_negative()) { ui.ErrorShow("ERROR NEGATIVE\nThe cut is dopped!"); return; }

			break;
	}


	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);

	LastUseChanging(request.type);

	prior.erase(ActiveIndex);
	CalculateDisplayVectorsByFourier(request.type);

	if (request.choice == UNTOUCHED)	{	prior.SetLastUse(&lastUse);	}	// N log( N )-es rendezes van benne -> nem szeretnenk mindig lefuttatni csak mikor kell

	GetPriorResult();
	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

	LOG("\tACCEPT :       " << request.type << "\n\n");

}

/* Amikor egy vágást kérünk ez a függvény fut le */
void Visualization::GetResult()
{
	/* Folyamat:
		Leenerőlizzük van e közös pontja az aktuális atommal -> HIBA ha nincs
		Vágás.
		2D-s kép lekérése
		HA nem MANUAL módban vagyunk, automatizáljuk az acceptet (van e közös rész a céltesttel?)
			HA MANUAL módban vagyunk, akkor 
					a megjelenítéshez mergeljük a 2 részt az eddig meglévővel olyam módon hogy az indexek ne csússzanak el elfogadás után
												-> old , negative, old , positive
					majd felöltjük ezt az adatot
	*/

	if (!app.container().atoms(ActiveAtom).intersects_plane(p))
	{ 
			LOG("\tCUT :  WRONG PLANE -> Cant cut that atom!\n");
			ui.RequestWrongCuttingErrorResolve();
			return; 
	}

	app.container().cut(ActiveAtom, p);

	Get2DDrawInfo();

	if (request.cut_mode != MANUAL) { 
		
		LOG("\tCUT: sík - ( " <<p.normal().x << " , " << p.normal().y << " , " << p.normal().z << " ) "
			<< "\n\t\t pont - ( " << p.example_point().x << " , " << p.example_point().y << " , " << p.example_point().z << " )\n");

		CutChecker();

		return;
	}

	LOG("\tCUT: sík - ( " << request.plane_norm.x << " , " << request.plane_norm.y << " , " << request.plane_norm.z << " ) "
		<< "\n\t\t pont - ( " << request.plane_coord.x << " , " << request.plane_coord.y << " , " << request.plane_coord.z << " )\n");

	_MergeDataContainer(data, app.cut_drawinfo());
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);

}

/* Amikor visszavonást kérünk ez a függvény fut le
		CSAK MANUAL módban van lehetőség undora!	*/
void Visualization::GetUndo()
{
	/*Folyamat:
		Undozzuk a vágást
		A merger állapotát is undozzuk (újra lekérjük az állapotot, majd föltöltjük)
		*/
	app.container().last_cut_result().undo();

	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);

	liveAtoms.erase(NumberOfAtoms);
	_2D_Line1Ids_N.clear();
	_2D_Line1Ids_P.clear();
	_2D_Line2Ids_N.clear();
	_2D_Line2Ids_P.clear();
	_2D_TriIds_N.clear();
	_2D_TriIds_P.clear();

	LOG("\tUNDO\n");
}

/* Amikor restartot kérünk ez a függvény fut le*/
void Visualization::GetRestart()
{
	/* Mindent az eredeti állapotába állítunk!*/
	NumberOfAtoms = 1;

	ActiveAtom = 0;
	ActiveIndex = 0;

	app.restart();
	PlaneCalculator->SetData(&app.container());
	PlaneCalculator->SetActive(ActiveAtom);

	data = app.atom_drawinfo();
	ObjectCreator::Create3DObject(data.points, data.indicies, _3DvaoID, _3DvboID, _3Dindex);

	prior.clear();

	liveAtoms.clear();
	relevantAtoms.clear();
	CalculateDisplayVectorsByFourier(NEGATIVE);
	display = &liveAtoms;

	lastUse.clear();
	lastUse.push_back(0);
	if (request.choice == UNTOUCHED) { prior.SetLastUse(&lastUse); }

	GetPriorResult();

	PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::Manual);
	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

	_2D_Line1Ids_N.clear();
	_2D_Line1Ids_P.clear();
	_2D_Line2Ids_N.clear();
	_2D_Line2Ids_P.clear();
	_2D_TriIds_N.clear();
	_2D_TriIds_P.clear();

	LOG("\tRESTART\n");
}

/* Amikor megváltozik a vágósík, ez a függvény fut le (only Manual mode)*/
void Visualization::SetNewPlane()
{
	PlaneCalculator->SetRequest(Utility::PlaneResult(request.plane_norm, request.plane_coord));

	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

}

/* Amikor megváltozik a stratégia, ez a függvény fut le*/
void Visualization::SetNewStrategy()
{
	/*Folyamat:
		Összehasonlító módosítása
		prioritásos sor törlése, feltöltése -> beszúró rendezés lesz
		Eredmény lekérés
		Vágósík számolás ??? (kell)*/
	switch (request.choice)
		{
			case VOLUME:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetVolume);
				prior.clear();
				for (GLuint i = 0;i < NumberOfAtoms;++i){	prior.insert(i, &app.container().atoms(i));	}
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : VOLUME\n");
				break;
			case DIAMETER:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetDiamaterLength);
				prior.clear();
				for (GLuint i = 0;i < NumberOfAtoms;++i)	{	prior.insert(i, &app.container().atoms(i));}
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : DIAMETER\n");
				break;
			case UNTOUCHED:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetLastUse);
				prior.clear();
				prior.SetLastUse(&lastUse);
				for (GLuint i = 0;i < NumberOfAtoms;++i) { prior.insert(i, &app.container().atoms(i)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : UNTOUCHED\n");
				break;
			case OPTIMALPARAMETER:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetOptimal);
				prior.clear();
				for (GLuint i = 0;i < NumberOfAtoms;++i) { prior.insert(i, &app.container().atoms(i)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : OPTIMALPARAMETER\n");
				break;
			case OPTIMALATMERO:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetOptimalAndDiameter);
				prior.clear();
				for (GLuint i = 0;i < NumberOfAtoms;++i) { prior.insert(i, &app.container().atoms(i)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : OPTIMALATMERO\n");
				break;
			case OPTIMALVOLUME:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetOptimalAndVolume);
				prior.clear();
				for (GLuint i = 0;i < NumberOfAtoms;++i) { prior.insert(i, &app.container().atoms(i)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : OPTIMALVOLUME\n");
				break;

		}
}

/* Amikor megváltozik a vágósík típusa, ez a függvény fut le*/
void Visualization::SetNewCuttingMode()
{
	/*Folyamat:
		Vágósík meghatározó fv. cseréje
		Új vágósík lekérés
		Log		*/
	switch (request.cut_mode)
	{
	case MANUAL:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::Manual);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : MANUAL\n");
		break;
	case RANDOMNORMALCENTROID:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomNormalCentroid);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : RANDOMNORMALCENTROID\n");
		break;
	case ATMEROSULYP:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::DiameterSulyp);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : ATMEROREMEROLEGESSULYP\n");
		break;
	case RANDOMLAPALATT:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomUnderFace);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : RANDOMLAPALATT\n");
		break;
	case MINDENPONTRAILLESZTETT:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::AllPointsFitting);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : MINDENPONTRAILLESZTETT\n");
		break;
	case RANDOMFELULETILLESZT:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomSurface);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : RANDOMFELULETILLESZT\n");
		break;
	}
}

/* Amikor megváltozik a látható atomok típusa (élő vs releváns), ez a függvény fut le*/
void Visualization::SetNewDisplayMode()
{
	display = request.disp == LIVE ? &liveAtoms : &relevantAtoms;
}

/* Amikor a sorban következő atomot kérjük, ez a függvény fut le*/
void Visualization::NextAtom()
{
	ActiveIndex = (ActiveIndex + 1) % liveAtoms.size();
	ActiveAtom = priorQue[ActiveIndex];

	PlaneCalculator->SetActive(ActiveAtom);

	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

	std::cout << "AKTIV: " << ActiveAtom << "\n";
	LOG("\tATOMVALTAS: +\n\n");
}

/* Amikor a sorban az aktuális előtti atomot kérjük, ez a függvény fut le*/
void Visualization::PrevAtom()
{ 
	ActiveIndex = (ActiveIndex - 1 + (int)liveAtoms.size()) % (int)liveAtoms.size();	// % nem kezeli a negatív számokat ()
	ActiveAtom = priorQue[ActiveIndex];

	PlaneCalculator->SetActive(ActiveAtom);

	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

	std::cout << "AKTIV: " << ActiveAtom << "\n";
	LOG("\tATOMVALTAS: -\n\n");
}

/*2D-s shaderek bekötése : vertex + fragment*/
void Visualization::Add2DShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "Shaders/vert_2d.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "Shaders/frag_2d.frag");

	program2D_ID = glCreateProgram();

	glAttachShader(program2D_ID, vs_ID);
	glAttachShader(program2D_ID, fs_ID);

	glBindAttribLocation(program2D_ID, 0, "vs_in_pos");
	
	glLinkProgram(program2D_ID);

	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);

	m_matProj = glm::perspective(45.0f, 800 / 600.0f, 0.01f, 1000.0f);

}

/*3D-s shaderek bekötése : vertex + geometry + fragment*/
void Visualization::Add3DShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "Shaders/vert_3d.vert");
	GLuint gs_ID = loadShader(GL_GEOMETRY_SHADER, "Shaders/geometry_shader.geom");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "Shaders/frag_3d.frag");

	program3D_ID = glCreateProgram();

	glAttachShader(program3D_ID, vs_ID);
	glAttachShader(program3D_ID, gs_ID);
	glAttachShader(program3D_ID, fs_ID);

	glBindAttribLocation(program3D_ID, 0, "vs_in_pos");

	glLinkProgram(program3D_ID);

	glDeleteShader(vs_ID);
	glDeleteShader(gs_ID);
	glDeleteShader(fs_ID);

	m_matProj = glm::perspective(45.0f, 800 / 600.0f, 0.01f, 1000.0f);


}

/*Uniform változók bekötése a shaderekhez*/
void Visualization::AddShaderUniformLocations()
{
	m_loc_mvp = glGetUniformLocation(program3D_ID, "MVP");
	m_loc_world = glGetUniformLocation(program3D_ID, "world");
	m_loc_worldIT = glGetUniformLocation(program3D_ID, "worldIT");
	eyePos = glGetUniformLocation(program3D_ID, "EyePosition");
	Lights = glGetUniformLocation(program3D_ID, "LightDirection");
	View = glGetUniformLocation(program3D_ID, "View");
	Opacity = glGetUniformLocation(program3D_ID, "opacity");
	DifCol = glGetUniformLocation(program3D_ID, "MaterialDiffuseColor");
	SpecCol = glGetUniformLocation(program3D_ID, "MaterialSpecularColor");

	m_loc_mvp2 = glGetUniformLocation(program2D_ID, "MVP");
	color2D = glGetUniformLocation(program2D_ID, "COLOR");
	alpha2D = glGetUniformLocation(program2D_ID, "ALPHA");

}

/*3D-s objektum rajzolása*/
void Visualization::Draw3D(const int& which, const bool& backdropping, glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
{
	/* Folyamat:
		Hátlapeldobás???
		vao BIND
		Egyéb tulajdonságok (szín, átlátsz.) felöltése uniform vált. keresztül
		Range meghatározása
		Rajzolás :)
		*/
	/*if (backdropping)	
	{	
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	*/
	glPolygonMode(GL_FRONT, _3DWireType ? GL_LINE : GL_FILL);

	glBindVertexArray(_3DvaoID);

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

/*2D-s információkat itt dolgozom föl
Negative vágási szelet feldolgozása
Positive vágási szelet feldolgozása
*/
void Visualization::Get2DDrawInfo()
{
	/*Default: negative rész*/
	_2DTri = &_2D_TriIds_N;
	_2DLine1 = &_2D_Line1Ids_N;
	_2DLine2 = &_2D_Line2Ids_N;

	//****************************************************************************
	_2Ddata = approx::drawinfo2d(*app.container().last_cut_result().negative());

	std::vector<glm::vec2> points;

	Active2DIndex = 0;

	_2D_Line1Ids_N.clear();
	_2D_Line2Ids_N.clear();
	_2D_TriIds_N.clear();

	_2D_Line1Ids_N.resize(_2Ddata.size());
	_2D_Line2Ids_N.resize(_2Ddata.size());

	for (int i = 0; i < _2Ddata.size(); ++i)
	{
		points.clear();
		size_t sizeOfRanges = _2Ddata[i].ranges.size();

		//***********************************************
		int start = _2Ddata[i].ranges[0];
		int end = _2Ddata[i].ranges[1];

		glm::vec2 point = _2Ddata[i].points[start];
		float maxx, maxy;
		float minx = maxx = point.x, miny = maxy = point.y;
		glm::vec2 eye = point;
		points.push_back(point);

		for (int j = start + 1; j < end;++j)
		{
			point = _2Ddata[i].points[j];

			minx = point.x < minx ? point.x : minx;
			miny = point.y < miny ? point.y : miny;
			maxx = point.x > maxx ? point.x : maxx;
			maxy = point.y > maxy ? point.y : maxy;

			eye += point;
			points.push_back(point);
		}

		_2D_TriIds_N.push_back(IdsAndVertC((int)points.size(), glm::vec3(eye.x / points.size(), 2 * sqrt(pow(minx - maxx, 2) + pow(miny - maxy, 2)), eye.y / points.size())));

		ObjectCreator::Create2DObject(points, _2D_TriIds_N[i].VaoId, _2D_TriIds_N[i].VboId);
		//***********************************************

		for (int j = 1; j < sizeOfRanges - 1; ++j)
		{
			points.clear();

			start = _2Ddata[i].ranges[j];
			end = _2Ddata[i].ranges[j + 1];

			for (int j = start; j < end;++j)
			{
				points.push_back(_2Ddata[i].points[j]);
			}
			points.push_back(_2Ddata[i].points[start]);

			switch (_2Ddata[i].outer[j])
			{
			case 0:
				_2D_Line1Ids_N[i].push_back(IdsAndVertC((int)points.size()));
				ObjectCreator::Create2DObject(points, _2D_Line1Ids_N[i].back().VaoId, _2D_Line1Ids_N[i].back().VboId);
				break;
			case 1:
				_2D_Line2Ids_N[i].push_back(IdsAndVertC((int)points.size()));
				ObjectCreator::Create2DObject(points, _2D_Line2Ids_N[i].back().VaoId, _2D_Line2Ids_N[i].back().VboId);
				break;
			}

		}
	}

	//*****************************************************************************
	//POSITIVE

	_2Ddata = approx::drawinfo2d(*app.container().last_cut_result().positive());

	/* Ezekbol tobb is lehet - kell a resize*/
	_2D_Line1Ids_P.clear();
	_2D_Line2Ids_P.clear();
	_2D_TriIds_P.clear();

	_2D_Line1Ids_P.resize(_2Ddata.size());
	_2D_Line2Ids_P.resize(_2Ddata.size());

	for (int i = 0; i < _2Ddata.size(); ++i)
	{
		points.clear();
		size_t sizeOfRanges = _2Ddata[i].ranges.size();

		//***********************************************
		int start = _2Ddata[i].ranges[0];
		int end = _2Ddata[i].ranges[1];

		glm::vec2 point = _2Ddata[i].points[start];
		float maxx, maxy;
		float minx = maxx = point.x, miny = maxy = point.y;
		glm::vec2 eye = point;
		points.push_back(point);

		for (int j = start + 1; j < end;++j)
		{
			point = _2Ddata[i].points[j];

			minx = point.x < minx ? point.x : minx;
			miny = point.y < miny ? point.y : miny;
			maxx = point.x > maxx ? point.x : maxx;
			maxy = point.y > maxy ? point.y : maxy;

			eye += point;
			points.push_back(point);
		}

		_2D_TriIds_P.push_back(IdsAndVertC((int)points.size(), glm::vec3(eye.x / points.size(), 2 * sqrt(pow(minx - maxx, 2) + pow(miny - maxy, 2)), eye.y / points.size())));

		ObjectCreator::Create2DObject(points, _2D_TriIds_P[i].VaoId, _2D_TriIds_P[i].VboId);
		//***********************************************

		for (int j = 1; j < sizeOfRanges - 1; ++j)
		{
			points.clear();

			start = _2Ddata[i].ranges[j];
			end = _2Ddata[i].ranges[j + 1];

			for (int j = start; j < end;++j)
			{
				points.push_back(_2Ddata[i].points[j]);
			}
			points.push_back(_2Ddata[i].points[start]);

			switch (_2Ddata[i].outer[j])
			{
			case 0:
				_2D_Line1Ids_P[i].push_back(IdsAndVertC((int)points.size()));
				ObjectCreator::Create2DObject(points, _2D_Line1Ids_P[i].back().VaoId, _2D_Line1Ids_P[i].back().VboId);
				break;
			case 1:
				_2D_Line2Ids_P[i].push_back(IdsAndVertC((int)points.size()));
				ObjectCreator::Create2DObject(points, _2D_Line2Ids_P[i].back().VaoId, _2D_Line2Ids_P[i].back().VboId);
				break;
			}

		}
	}
}

/*2D-s metszet vetületek rajzolása*/
void Visualization::Draw2D(/*const int& NumbersOfVertices, */glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
{
	/*Folyamat:
		Vonalak rajzolása (targetatommal való metszet)
		Metszet vetület átlátszóan	*/
	if (!_2DTri->size()) return;

	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glDisable(GL_BLEND);
	IdsAndVertC tmp;

	//***********************************************************

	glUniform3f(color2D, 0.0f, 0.0f, 1.0f);
	glUniform1f(alpha2D, 1.0f);

	for (int i = 0;  (*_2DLine1).size()!=0 &&  i < (*_2DLine1)[Active2DIndex].size(); ++i)
	{
		tmp = (*_2DLine1)[Active2DIndex][i];
		glBindVertexArray(tmp.VaoId);

		glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

		glDrawArrays(GL_LINE_STRIP,
			0,
			tmp.count);
	}
	
	//***********************************************************

	glUniform3f(color2D, 0.0f, 1.0f, 0.0f);
	glUniform1f(alpha2D, 1.0f);

	for (int i = 0;  (*_2DLine2).size() != 0 &&  i < (*_2DLine2)[Active2DIndex].size(); ++i)
	{
		tmp = (*_2DLine2)[Active2DIndex][i];
		glBindVertexArray(tmp.VaoId);

		glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

		glDrawArrays(GL_LINE_STRIP,
			0,
			tmp.count);
	}
	
	//**************************************************
	glEnable(GL_BLEND);
	glUniform3f(color2D, 1.0f, 0.0f, 0.0f);
	glUniform1f(alpha2D, 0.5f);

	tmp = (*_2DTri)[Active2DIndex];
	glBindVertexArray(tmp.VaoId);

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));

	glDrawArrays(GL_TRIANGLE_FAN,
		0,
		tmp.count);
}

/*Target test rajzolása*/
void Visualization::DrawTargetBody()
{
	glPolygonMode(GL_FRONT, _3DWireType ? GL_LINE : GL_FILL);

	glBindVertexArray(_target_vaoID);

	glm::mat4 matWorld = glm::mat4(1.0f);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));

	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(matWorld[0][0]));
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &(matWorldIT[0][0]));

	glUniform1f(Opacity, 1.0f);
	SetTargetAtomProperties();

	glDrawElements(GL_TRIANGLES,
		(GLsizei)targetdata.indicies.size(),
		GL_UNSIGNED_SHORT,
		0);
}

/*Vágósík rajzolása*/
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
		3 * 2 * CuttingPlaneFreq * CuttingPlaneFreq,		
		GL_UNSIGNED_SHORT,	
		0);					
}

/*Fourier együttható szerinti csoportba kerülés
	liveAtoms = élő atomok : nem 0 és nem 1 fourier együttható
	relevansAtom = releváns atomok: FOURIERCOEFFICIENT (0.5) -nél nagyobb fourier egy.ú atomok*/
void Visualization::CalculateDisplayVectorsByFourier(const TypeOfAccept& ta)
{
	/* ActiveAtom negativ fele ; NumberOfAtoms-1 positive fele both esetén*/
	float fourier = app.container().atoms(ActiveAtom).fourier();
	liveAtoms.erase(ActiveAtom); // EF a vágásnak
	relevantAtoms.erase(ActiveAtom);

	/* MANUALNAL lesz erdekeltsege, ha nem manual akkor ilyen nincs is
	   Ahhoz hogy lassuk berakjuk az elo atomok koze mindket reszt, viszont annak ki kell kerulnie (a negativ mindenkepp ki fog)
	   DE ha BOTH-t tartjuk meg akkor a NumberOfAtoms valtozni fog -> alkalmazkodunk (kodismetles helyett ezt valasztottam)
	*/
	liveAtoms.erase(ta == BOTH ? NumberOfAtoms - 1 : NumberOfAtoms); // EF a vágásnak
	relevantAtoms.erase(ta == BOTH ? NumberOfAtoms - 1 : NumberOfAtoms);	//Van közös rész, de újabb vágás esetén már nem biztos hogy releváns lesz -> need this


	if ( std::abs(fourier) > EPSILONFOURIER && std::abs(fourier-1.0f) > EPSILONFOURIER)
	{
		switch (ta)
		{
			case BOTH:
				prior.insert(ActiveAtom, &app.container().atoms(ActiveAtom));
				liveAtoms.insert(ActiveAtom);
				break;
			case NEGATIVE:
				prior.insert(ActiveAtom, &app.container().atoms(ActiveAtom));
				liveAtoms.insert(ActiveAtom);
				break;
			case POSITIVE:
				/*Eltunik ez a resz*/
				break;
		}
	}
	if (fourier > FOURIERCOEFFICIENT)
	{
		relevantAtoms.insert(ActiveAtom);
	}

	if (ta == NEGATIVE) return;

	fourier = app.container().atoms(NumberOfAtoms-1).fourier();
	if (std::abs(fourier) > EPSILONFOURIER && std::abs(fourier - 1.0f) > EPSILONFOURIER)
	{
		switch (ta)
		{
		case BOTH:
			prior.insert(NumberOfAtoms - 1, &app.container().atoms(NumberOfAtoms - 1));
			liveAtoms.insert(NumberOfAtoms - 1);
			break;
		case NEGATIVE:
			/*Eltunik ez a resz*/
			break;
		case POSITIVE:
			prior.insert(ActiveAtom, &app.container().atoms(ActiveAtom));
			liveAtoms.insert(ActiveAtom);
			break;
		}
	}
	if (fourier > FOURIERCOEFFICIENT)
	{
		relevantAtoms.insert(NumberOfAtoms - 1);
	}
}

/*Automatizált elfogadó:
Vágás után lecheckoljuk van e metszete az egyes részeknek a céltesttel
Epszilon: 0.001					*/
void Visualization::CutChecker()
{
	bool IntersectionWithNegative = app.container().last_cut_result().negative()->intersection_volume() > 0.001;
	bool IntersectionWithPositive = app.container().last_cut_result().positive()->intersection_volume() > 0.001;

	if (IntersectionWithNegative && IntersectionWithPositive)	request.type = BOTH;
	else if (IntersectionWithNegative) request.type = NEGATIVE;
	else request.type = POSITIVE;

	AcceptCutting();
}

/*Prioritásos sor tartalmát lekérő eljárás*/
void Visualization::GetPriorResult()
{
	std::vector<Utility::PriorResult> result = prior.GetOrder();
	priorQue = prior.GetPriorIndexes();

	ActiveIndex = 0;
	ActiveAtom = priorQue.size() > 0 ? priorQue[ActiveIndex] : 0;

	PlaneCalculator->SetActive(ActiveAtom);

	if (logger) {
		std::for_each(result.begin(), result.end(), Utility::writer());
		std::cout << "\n";
	}

}

/*Megállapítja hogy az aktuális atomról van-e szó mindegy milyen módban is vagyunk!
Megj.: Fontosak az indexek*/
bool Visualization::IsItActive(const int& which)
{
	return	(request.type == NEGATIVE && (ActiveAtom) == which)
		|| (request.type == POSITIVE && NumberOfAtoms == which)
		|| (request.type == BOTH && (NumberOfAtoms == which || (ActiveAtom) == which));
}

/*Legrégebben használt atomok stratégiához
	Ez aktualizálja a lastuse vektorom		*/
void Visualization::LastUseChanging(const TypeOfAccept& ta)
{
	for (size_t i = 0; i < lastUse.size();++i) lastUse[i]++;

	switch (ta)
	{
		case BOTH:
			lastUse[ActiveAtom] = 0;
			lastUse.push_back(0);
			break;
		case POSITIVE:
			lastUse.erase(lastUse.begin() + ActiveAtom);
			lastUse.push_back(0);
			break;
		case NEGATIVE:
			lastUse[ActiveAtom] = 0;
		break;
	}
	
}

/*	MANUAL módban a vágási eredmény mergelése a többi szabad atommal
Folyamat:
Search our points which only used in the cutted atom -> collect them in a set
Delete them in reverse mode and decrease our indexes
Delete our unused indicies
Insert our ponts
Insert Indicies in 3 part , before start , new indicies part1, after end, new indicies part2
Insert Ranges in 3 part with same method
*/
void Visualization::_MergeDataContainer(approx::BodyList& data, const approx::BodyList& cutresult)
{
	liveAtoms.insert(NumberOfAtoms);	// ez lesz azaz index ami a végére kerül

	int start = data.index_ranges[ActiveAtom];
	int end = data.index_ranges[ActiveAtom + 1];
	//Pontok törlése, indexek javítása
	std::set<int> DeletedPoints;

	for (int i = start; i < end - 1; ++i)
	{
		auto count = 0;
		for (int j = 0; j < start; ++j)
		{
			if (data.indicies[i] == data.indicies[j]) count++;
		}
		for (size_t j = end; j < data.indicies.size(); ++j)
		{
			if (data.indicies[i] == data.indicies[j]) count++;
		}

		auto index = data.indicies[i];
		if (!count /*&& DeletedPoints.count(index) == 0*/)	//torolheto
		{
			DeletedPoints.insert(index);
		}
	}

	//Set orderd min to max by default and we want to erase in reverse mode to get the correct result
	for (std::set<int>::reverse_iterator j = DeletedPoints.rbegin(); j != DeletedPoints.rend(); j++)
	{
		data.points.erase(data.points.begin() + *j);
		for (std::vector<GLushort>::iterator k = data.indicies.begin(); k != data.indicies.end(); k++)
		{
			if (*k >= *j) { (*k)--; }
		}
	}

	//Indexek törlése ; In order to erase the correct index we should go back to front
	for (int i = end - 1; i >= start; --i)
	{
		data.indicies.erase(data.indicies.begin() + i);
	}

	//--------------------------- EPITES
	auto CountOfPoints = data.points.size();

	//Pontok hozzafuzese
	for (std::vector<glm::vec3>::const_iterator it = cutresult.points.begin(); it != cutresult.points.end(); ++it)
	{
		data.points.push_back(*it);
	}

	//Indexek hozzafuzese
	std::vector<GLushort> new_indicies(data.indicies.begin(), data.indicies.begin() + start);

	for (int i = 0; i < cutresult.index_ranges[1];++i)
	{
		new_indicies.push_back(cutresult.indicies[i] + (int)CountOfPoints);
	}

	//már törölve vannak ezek az idnexek -> starttól
	for (size_t i = start; i < data.indicies.size();++i)
	{
		new_indicies.push_back(data.indicies[i]);
	}

	for (int i = cutresult.index_ranges[1]; i < cutresult.index_ranges[2];++i)
	{
		new_indicies.push_back(cutresult.indicies[i] + (int)CountOfPoints);
	}

	data.indicies = new_indicies;

	//Indexrange helyreallitas
	std::vector<GLushort> new_ranges(data.index_ranges.begin(), data.index_ranges.begin() + ActiveAtom + 1);

	int LastIndexRange = new_ranges.size() == 0 ? 0 : new_ranges[new_ranges.size() - 1];

	new_ranges.push_back(cutresult.index_ranges[1] + LastIndexRange);

	for (size_t i = ActiveAtom + 2; i < data.index_ranges.size();++i)
	{
		auto CutRange = data.index_ranges[i] - data.index_ranges[i - 1];
		auto last_element = new_ranges[new_ranges.size() - 1];
		new_ranges.push_back(last_element + CutRange);
	}

	auto CutRange = cutresult.index_ranges[2] - cutresult.index_ranges[1];

	new_ranges.push_back(CutRange + new_ranges[new_ranges.size() - 1]);

	data.index_ranges = new_ranges;
}

/*Vágósík beállítás + rajzoláshoz szükséges információk összeszedése
Draw method:
Change the default normal to the actual normal
Translate to centr - distance
where centr = centroid of atom,
distance = centroid distance from the plane	*/
void Visualization::RefreshPlaneData(const Utility::PlaneResult& newplanedata)
{
	p = approx::Plane<float>(newplanedata.normal, newplanedata.point);
	centr = app.container().atoms(ActiveAtom).centroid();
	distance = p.classify_point(centr);
	_planenormal = approx::convert(p.normal());
}

//Mouse/Keyboard eventek
void Visualization::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym) {
	case SDLK_p:	// change projections
		Active2DIndex = 0;
		c.SwitchCameraView(_2DTri->size() ? (*_2DTri)[Active2DIndex].eye : glm::vec3(1, 2, 1));
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
	case SDLK_l:	//logger
		logger = !logger;
		if (logger)
		{
			std::vector<Utility::PriorResult> result = prior.GetOrder();
			std::for_each(result.begin(), result.end(), Utility::writer());
			std::cout << "\n";
		}
		break;
	case SDLK_KP_PLUS:	//2D-s lapváltó
		if (!c.Is2DView() || !_2DTri->size()) break;
		Active2DIndex = (Active2DIndex + 1) % (*_2DTri).size();
		c.SetCamera((*_2DTri)[Active2DIndex].eye);
		break;
	case SDLK_KP_MINUS:
		if (!c.Is2DView() || !_2DTri->size()) break;
		Active2DIndex = (Active2DIndex - 1 + (int)(*_2DTri).size()) % (int)(*_2DTri).size();
		c.SetCamera((*_2DTri)[Active2DIndex].eye);
		break;
	case SDLK_o:	//2D-s pos - neg váltó
		if (!c.Is2DView() || !_2DTri->size()) break;
		Active2DIndex = 0;
		_2DLine1 = _2DLine1 == &_2D_Line1Ids_N ? &_2D_Line1Ids_P : &_2D_Line1Ids_N;
		_2DLine2 = _2DLine2 == &_2D_Line2Ids_N ? &_2D_Line2Ids_P : &_2D_Line2Ids_N;
		_2DTri = _2DTri == &_2D_TriIds_N ? &_2D_TriIds_P : &_2D_TriIds_N;
		c.SetCamera((*_2DTri)[Active2DIndex].eye);
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
	if (mouse.button == SDL_BUTTON_LEFT) {
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
	wheel.y > 0 ? (c.Add(c.GetZoomUnit())) : (c.Sub(c.GetZoomUnit()));
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
	glUniform3f(DifCol, 0.0f, 0.5f, 0.5f);
	glUniform3f(SpecCol, 0.0f, 0.2f, 0.2f);
}


/*Frissítés:
Nézeti transzf. mátrix
UI ról jövő request	*/
void Visualization::Update()
{
	m_matView = glm::lookAt(c.GetEye(), c.GetAt(), c.GetUp());

	request = ui.GetRequest();
}
/*Clean
TODO: aktualizálni!*/
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

	glDeleteProgram(program3D_ID);

	LOG("------------------------- END -----------------------------------\n\n\n");
}
/*Resize: ablakátméretezés*/
void Visualization::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	// nyilasszog, ablakmeret nezeti arany, kozeli és tavoli vagosik
	m_matProj = glm::perspective(45.0f, _w / (float)_h, 0.01f, 1000.0f);
}
