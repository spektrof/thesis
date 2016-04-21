#include "vis.h"
#include "../Utils/GLUtils.hpp"

#include <GL/GLU.h>

#define CuttingPlaneWireType 1
#define _3DWireType 0
#define FOURIERCOEFFICIENT 0.5f
#define EPSILONFOURIER 0.0001f
#define INTERSECTIONEPSILON  0.001f

Visualization::Visualization(void)
{
	program2D_ID = 0;
	program3D_ID = 0;

	CuttingPlaneFreq = 12;

	filename = "Targets/gummybear.obj";
	PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::Manual);

	_2DTri = &_2D_TriIds_N;
	_2DLine1 = &_2D_Line1Ids_N;
	_2DLine2 = &_2D_Line2Ids_N;
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

	ObjectCreator::CreateCuttingPlane(planeIds.VaoId, planeIds.VboId, planeIds.IndexId, 50, CuttingPlaneFreq);	// (x,y síkban fekvő , (0,0,1) normálisú négyzet)
	 
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
	if (!app.set_target(filename.c_str(), 10.0f, 0.0f, 20.0f)) {
		LOG("HIBA A FAJL BETOLTESENEL!\n");
		return false;
	}

	NumberOfAtoms = 1;
	ActiveAtom = 0;
	ActiveIndex = 0;

	adj_mtx = Utility::GetAdjacencyMatrix(&(app.target().face_container()));

	if (PlaneCalculator != NULL)
	{
		delete PlaneCalculator;
	}
	PlaneCalculator = new PlaneGetterFunctions<approx::Approximation<float>>(&app.container(),&adj_mtx);

	data = app.atom_drawinfo();
	targetdata = app.target_drawinfo();

	CleanIdBufferForReuse(_3dIds);
	//CleanIdBufferForReuse(targetIds);
	ObjectCreator::Create3DObject(data.points, data.indicies, _3dIds.VaoId, _3dIds.VboId, _3dIds.IndexId);
	ObjectCreator::Create3DObject(targetdata.points, targetdata.indicies,targetIds.VaoId, targetIds.VboId, targetIds.IndexId);
	
	lastUse.clear();
	lastUse.push_back(0);
	prior.clear();
	prior.SetLastUse(&lastUse);

	liveAtoms.clear();
	relevantAtoms.clear();
	CalculateDisplayVectorsByFourier();
	display = &liveAtoms;

	GetPriorResult();
	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());

	return true;
}

/* Renderelo : 2D vagy 3D ben ábrázol 
		3D-ben fogad requesteket!
*/
void Visualization::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
		if (c.Is2DView()) c.SwitchCameraView();
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
		app.container().garbage_collection();
		break;
	case EXPORT:
		app.save_atoms("Result/save_atoms.obj");
		app.save_approximated_body("Result/save_atom_appbody.obj");
		app.container().final_transform();
		/*app.save_atoms("save_atoms");
		app.save_atoms("save_atoms");*/
		break;
	case IMPORT:
		ImportNewTarget();
		break;
	default:
		break;
	}

	if (c.Is2DView())	// true when _2D
	{
		glUseProgram(program2D_ID);
		
		Draw2D();
	}
	else // _3D
	{
		glUseProgram(program3D_ID);

		glUniform3fv(eyePos, 1, glm::value_ptr(c.GetEye()));
		glUniform3fv(Lights, 1, glm::value_ptr(l.GetLightDir()));
	
		DrawCuttingPlane(Utility::GetTranslate(centr,_planenormal,distance),Utility::GetRotateFromNorm(_planenormal));
						
		DrawTargetBody();

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
			if (logger) std::cout << "Megtartom: BOTH\n";

			NumberOfAtoms++;
			break;
		case POSITIVE:
			if (!app.container().last_cut_result().choose_positive()) { ui.ErrorShow("ERROR POSITIVE\nThe cut is dopped!"); return; }
			if (logger) std::cout << "Megtartom: POSITIVE\n";

			break;
		case NEGATIVE:
			if (!app.container().last_cut_result().choose_negative()) { ui.ErrorShow("ERROR NEGATIVE\nThe cut is dopped!"); return; }
			if (logger) std::cout << "Megtartom: NEGATIVE\n";

			break;
	}


	data = app.atom_drawinfo();

	CleanIdBufferForReuse(_3dIds);
	ObjectCreator::Create3DObject(data.points, data.indicies, _3dIds.VaoId, _3dIds.VboId, _3dIds.IndexId);

	LastUseChanging(request.type);

	if (request.choice == UNTOUCHED)	{	
		prior.SetLastUse(&lastUse);
	}	// N log( N )-es rendezes van benne -> nem szeretnenk mindig lefuttatni csak mikor kell

	prior.erase(ActiveIndex);
	CalculateDisplayVectorsByFourier();

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
	
	if (!app.container().atoms(ActiveAtom).intersects_plane(p,0.001f))
	{ 
			LOG("\tCUT :  WRONG PLANE -> Cant cut that atom -> New plane generated!\n");
			RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
			if (request.eventtype == MORESTEPS) return;
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

	MergeDataContainer(data, app.cut_drawinfo());

	CleanIdBufferForReuse(_3dIds);
	ObjectCreator::Create3DObject(data.points, data.indicies, _3dIds.VaoId, _3dIds.VboId, _3dIds.IndexId);

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

	CleanIdBufferForReuse(_3dIds);
	ObjectCreator::Create3DObject(data.points, data.indicies, _3dIds.VaoId, _3dIds.VboId, _3dIds.IndexId);

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

	CleanIdBufferForReuse(_3dIds);
	ObjectCreator::Create3DObject(data.points, data.indicies, _3dIds.VaoId, _3dIds.VboId, _3dIds.IndexId);

	prior.clear();

	liveAtoms.clear();
	relevantAtoms.clear();
	CalculateDisplayVectorsByFourier();
	display = &liveAtoms;

	lastUse.clear();
	lastUse.push_back(0);
	if (request.choice == UNTOUCHED) { prior.SetLastUse(&lastUse); }

	GetPriorResult();

	PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::Manual);
	RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
	
	Release2DIds();

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
				for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it) {  prior.insert(*it, &app.container().atoms(*it));	}
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : VOLUME\n");
				break;
			case DIAMETER:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetDiamaterLength);
				prior.clear();
				for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it) { prior.insert(*it, &app.container().atoms(*it)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : DIAMETER\n");
				break;
			case UNTOUCHED:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetLastUse);
				prior.clear();
				prior.SetLastUse(&lastUse);
				for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it) { prior.insert(*it, &app.container().atoms(*it)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : UNTOUCHED\n");
				break;
			case OPTIMALPARAMETER:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetOptimal);
				prior.clear();
				for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it) { prior.insert(*it, &app.container().atoms(*it)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : OPTIMALPARAMETER\n");
				break;
			case OPTIMALDIAMETER:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetOptimalAndDiameter);
				prior.clear();
				for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it) { prior.insert(*it, &app.container().atoms(*it)); }
				GetPriorResult();
				RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
				LOG("Choice : OPTIMALATMERO\n");
				break;
			case OPTIMALVOLUME:
				prior.SetComparer(&SorterFunctions<approx::ConvexAtom<float>>::GetOptimalAndVolume);
				prior.clear();
				for (std::set<int>::iterator it = display->begin(); it != display->end(); ++it) { prior.insert(*it, &app.container().atoms(*it)); }
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
	case DIAMETERCENTROID:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::DiameterCentroid);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : ATMEROREMEROLEGESSULYP\n");
		break;
	case RANDOMUNDERFACE:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomUnderFace);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : RANDOMLAPALATT\n");
		break;
	case MATCHEDEACHPOINT:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::AllPointsFitting);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		LOG("CuttingMode : MINDENPONTRAILLESZTETT\n");
		break;
	case MATCHEDRANDOMSURFACE:
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

void Visualization::ImportNewTarget()
{
	std::string newfile = "Targets/" + request.filename + ".obj";
	if (newfile == filename) return;

	struct stat buffer;
	if (stat(newfile.c_str(), &buffer) == 0)
	{
		std::string tmp = filename;
		filename = newfile;

		if (EngineInit())
		{
			LOG("New file loaded successfully , new file is: " + newfile + "\n");
		}
		else
		{
			filename = tmp;
			LOG("New file loaded unsuccessfully\n");
		}
	}
	else {
		ui.ErrorShow("Invalid Filename!");
	}
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
	eyePos = glGetUniformLocation(program3D_ID, "EyePosition");
	world = glGetUniformLocation(program3D_ID, "World");
	Lights = glGetUniformLocation(program3D_ID, "LightDirection");
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

	glBindVertexArray(_3dIds.VaoId);

	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 mvp = m_matProj * m_matView * matWorld;
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(world, 1, GL_FALSE, &(matWorld[0][0]));

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

	Active2DIndex = 0;

	_2D_Line1Ids_N.clear();
	_2D_Line2Ids_N.clear();
	_2D_TriIds_N.clear();

	_2D_Line1Ids_N.resize(_2Ddata.size());
	_2D_Line2Ids_N.resize(_2Ddata.size());

	for (int i = 0; i < _2Ddata.size(); ++i)
	{
		size_t sizeOfRanges = _2Ddata[i].ranges.size();

		//***********************************************
		int start = _2Ddata[i].ranges[0];
		int end = _2Ddata[i].ranges[1];

		glm::vec2 point = _2Ddata[i].points[start];
		float maxx, maxy;
		float minx = maxx = point.x, miny = maxy = point.y;
		glm::vec2 eye = point;

		for (int j = start + 1; j < end;++j)
		{
			point = _2Ddata[i].points[j];

			minx = point.x < minx ? point.x : minx;
			miny = point.y < miny ? point.y : miny;
			maxx = point.x > maxx ? point.x : maxx;
			maxy = point.y > maxy ? point.y : maxy;

			eye += point;
		}

		_2D_TriIds_N.push_back(IdsAndProp(end-start, glm::vec3(eye.x / (end - start), 2 * sqrt(pow(minx - maxx, 2) + pow(miny - maxy, 2)), eye.y /( end - start))));

		CleanIdBufferForReuse(_2D_TriIds_N[i]);
		ObjectCreator::Create2DObject(std::vector<glm::vec2>(_2Ddata[i].points.begin() + start, _2Ddata[i].points.begin() + end), _2D_TriIds_N[i].VaoId, _2D_TriIds_N[i].VboId);
		//***********************************************

		for (int j = 1; j < sizeOfRanges - 1; ++j)
		{
			start = _2Ddata[i].ranges[j];
			end = _2Ddata[i].ranges[j + 1];
			std::vector<glm::vec2> points = std::vector<glm::vec2>(_2Ddata[i].points.begin() + start, _2Ddata[i].points.begin() + end);
			points.push_back(_2Ddata[i].points[start]);

			switch (_2Ddata[i].outer[j])
			{
			case 0:
				_2D_Line1Ids_N[i].push_back(IdsAndProp(end - start + 1));
				CleanIdBufferForReuse(_2D_Line1Ids_N[i].back());
				ObjectCreator::Create2DObject(points, _2D_Line1Ids_N[i].back().VaoId, _2D_Line1Ids_N[i].back().VboId);
				break;
			case 1:
				_2D_Line2Ids_N[i].push_back(IdsAndProp(end - start + 1));
				CleanIdBufferForReuse(_2D_Line2Ids_N[i].back());
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
		size_t sizeOfRanges = _2Ddata[i].ranges.size();

		//***********************************************
		int start = _2Ddata[i].ranges[0];
		int end = _2Ddata[i].ranges[1];

		glm::vec2 point = _2Ddata[i].points[start];
		float maxx, maxy;
		float minx = maxx = point.x, miny = maxy = point.y;
		glm::vec2 eye = point;

		for (int j = start + 1; j < end;++j)
		{
			point = _2Ddata[i].points[j];

			minx = point.x < minx ? point.x : minx;
			miny = point.y < miny ? point.y : miny;
			maxx = point.x > maxx ? point.x : maxx;
			maxy = point.y > maxy ? point.y : maxy;

			eye += _2Ddata[i].points[j];
		}

		_2D_TriIds_P.push_back(IdsAndProp((end - start), glm::vec3(eye.x / (end - start), 2 * sqrt(pow(minx - maxx, 2) + pow(miny - maxy, 2)), eye.y / (end - start))));

		CleanIdBufferForReuse(_2D_TriIds_P[i]);
		ObjectCreator::Create2DObject(std::vector<glm::vec2>(_2Ddata[i].points.begin()+start, _2Ddata[i].points.begin()+end), _2D_TriIds_P[i].VaoId, _2D_TriIds_P[i].VboId);
		//***********************************************

		for (int j = 1; j < sizeOfRanges - 1; ++j)
		{
			start = _2Ddata[i].ranges[j];
			end = _2Ddata[i].ranges[j + 1];

			std::vector<glm::vec2> points = std::vector<glm::vec2>(_2Ddata[i].points.begin() + start, _2Ddata[i].points.begin() + end);
			points.push_back(_2Ddata[i].points[start]);

			switch (_2Ddata[i].outer[j])
			{
			case 0:
				_2D_Line1Ids_P[i].push_back(IdsAndProp(end - start + 1));
				CleanIdBufferForReuse(_2D_Line1Ids_P[i].back());
				ObjectCreator::Create2DObject(points, _2D_Line1Ids_P[i].back().VaoId, _2D_Line1Ids_P[i].back().VboId);
				break;
			case 1:
				_2D_Line2Ids_P[i].push_back(IdsAndProp(end - start + 1));
				CleanIdBufferForReuse(_2D_Line2Ids_P[i].back());
				ObjectCreator::Create2DObject(points, _2D_Line2Ids_P[i].back().VaoId, _2D_Line2Ids_P[i].back().VboId);
				break;
			}

		}
	}

	if (c.Is2DView()) c.SetCamera((*_2DTri)[0].eye);

}

/*2D-s metszet vetületek rajzolása*/
void Visualization::Draw2D(glm::mat4& scal, glm::mat4& trans, glm::mat4& rot)
{
	/*Folyamat:
		Vonalak rajzolása (targetatommal való metszet)
		Metszet vetület átlátszóan	*/
	if (!_2DTri->size()) return;

	glm::mat4 matWorld = trans * rot * scal;
	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glDisable(GL_BLEND);
	IdsAndProp tmp;

	//***********************************************************

	glUniform3f(color2D, 0.0f, 0.0f, 1.0f);
	glUniform1f(alpha2D, 1.0f);

	for (int i = 0;  (*_2DLine1).size()!=0 &&  i < (*_2DLine1)[Active2DIndex].size(); ++i)
	{
		tmp = (*_2DLine1)[Active2DIndex][i];
		glBindVertexArray(tmp.VaoId);

		glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
		glUniformMatrix4fv(world, 1, GL_FALSE, &(matWorld[0][0]));

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
		glUniformMatrix4fv(world, 1, GL_FALSE, &(matWorld[0][0]));

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
	glUniformMatrix4fv(world, 1, GL_FALSE, &(matWorld[0][0]));

	glDrawArrays(GL_TRIANGLE_FAN,
		0,
		tmp.count);
}

/*Target test rajzolása*/
void Visualization::DrawTargetBody()
{
	glPolygonMode(GL_FRONT, _3DWireType ? GL_LINE : GL_FILL);

	glBindVertexArray(targetIds.VaoId);

	glm::mat4 matWorld = glm::mat4(1.0f);
	glm::mat4 mvp = m_matProj * m_matView * matWorld;
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(world, 1, GL_FALSE, &(matWorld[0][0]));

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
	glm::mat4 mvp = m_matProj * m_matView * matWorld;

	glBindVertexArray(planeIds.VaoId);

	SetPlaneProperties();
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(world, 1, GL_FALSE, &(matWorld[0][0]));

	glDrawElements(GL_TRIANGLES,		
		3 * 2 * CuttingPlaneFreq * CuttingPlaneFreq,		
		GL_UNSIGNED_SHORT,	
		0);					
}

/*Fourier együttható szerinti csoportba kerülés
	liveAtoms = élő atomok : nem 0 és nem 1 fourier együttható
	relevansAtom = releváns atomok: FOURIERCOEFFICIENT (0.5) -nél nagyobb fourier egy.ú atomok*/
void Visualization::CalculateDisplayVectorsByFourier()
{	//underface be is az epszilont ehez hangolni
	/* ActiveAtom negativ fele ; NumberOfAtoms-1 positive fele both esetén*/
	float fourier = app.container().atoms(ActiveAtom).fourier();
	if (logger) { std::cout << "Fourier of First Atom: " << fourier << "\n"; }

	if ( std::abs(fourier) > EPSILONFOURIER && std::abs(fourier-1.0f) > EPSILONFOURIER)
	{
		prior.insert(ActiveAtom, &app.container().atoms(ActiveAtom));
		liveAtoms.insert(ActiveAtom);
	}
	if (fourier > FOURIERCOEFFICIENT)
	{
		relevantAtoms.insert(ActiveAtom);
	}

	if (request.type != BOTH) return;

	fourier = app.container().atoms(NumberOfAtoms-1).fourier();
	if (logger) { std::cout << "Fourier of Second Atom: " << fourier << "\n"; }

	if (std::abs(fourier) > EPSILONFOURIER && std::abs(fourier - 1.0f) > EPSILONFOURIER)
	{
		prior.insert(NumberOfAtoms - 1, &app.container().atoms(NumberOfAtoms - 1));
		liveAtoms.insert(NumberOfAtoms - 1);
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
	bool IntersectionWithNegative = app.container().last_cut_result().negative()->intersection_volume() > INTERSECTIONEPSILON;
	bool IntersectionWithPositive = app.container().last_cut_result().positive()->intersection_volume() > INTERSECTIONEPSILON;

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
		std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << a.id << " " << a.value << "\n"; });
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

	liveAtoms.erase(ActiveAtom); // EF a vágásnak
	relevantAtoms.erase(ActiveAtom);

	/* MANUALNAL lesz erdekeltsege, ha nem manual akkor ilyen nincs is
	Ahhoz hogy lassuk berakjuk az elo atomok koze mindket reszt, viszont annak ki kell kerulnie (a negativ mindenkepp ki fog)
	DE ha BOTH-t tartjuk meg akkor a NumberOfAtoms valtozni fog -> alkalmazkodunk (kodismetles helyett ezt valasztottam)
	*/
	liveAtoms.erase(request.type == BOTH ? NumberOfAtoms - 1 : NumberOfAtoms); // EF a vágásnak
	relevantAtoms.erase(request.type == BOTH ? NumberOfAtoms - 1 : NumberOfAtoms);	//Van közös rész, de újabb vágás esetén már nem biztos hogy releváns lesz -> need this
	
	switch (ta)
	{
		case BOTH:
			lastUse[ActiveAtom] = 0;
			lastUse.push_back(0);
			break;
		case POSITIVE:
			lastUse[ActiveAtom] = 0;
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
void Visualization::MergeDataContainer(approx::BodyList& data, const approx::BodyList& cutresult)
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
	case SDLK_1:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::AllPointsFitting);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_2:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::AllPointsFitting2);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_3:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::AllPointsFitting3);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_4:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::AllPointsFitting4);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_5:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomSurface);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_6:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomSurface2);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_7:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomSurface3);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
	case SDLK_8:
		PlaneFunction = PlaneGetter(&PlaneGetterFunctions<approx::Approximation<float>>::RandomSurface4);
		RefreshPlaneData((PlaneCalculator->*PlaneFunction)());
		break;
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
		c.Add(c.GetHorUnit());
		break;
	case SDLK_d:
		c.Sub(c.GetHorUnit());
		break;
	case SDLK_DOWN:	//feny
		l.AddTo(l.GetTheta());
		break;
	case SDLK_UP:
		l.SubFrom(l.GetTheta());
		break;
	case SDLK_LEFT:
		l.AddTo(l.GetOmega());
		break;
	case SDLK_RIGHT:
		l.SubFrom(l.GetOmega());
		break;
	case SDLK_l:	//logger
		logger = !logger;
		if (logger)
		{
			std::vector<Utility::PriorResult> result = prior.GetOrder();
			std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << a.id << " " << a.value << "\n"; });
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
	CleanIdBufferForReuse(_3dIds);
	CleanIdBufferForReuse(planeIds);
	CleanIdBufferForReuse(targetIds);
	Release2DIds();

	glDeleteProgram(program3D_ID);

	delete PlaneCalculator;
}
/*Resize: ablakátméretezés*/
void Visualization::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	// nyilasszog, ablakmeret nezeti arany, kozeli és tavoli vagosik
	m_matProj = glm::perspective(45.0f, _w / (float)_h, 0.01f, 1000.0f);
}

void Visualization::CleanIdBufferForReuse(const IdsAndProp iap)
{
	glDeleteVertexArrays(1, &iap.VaoId);
	glDeleteBuffers(1, &iap.IndexId);
	glDeleteBuffers(1, &iap.VboId);
}

void Visualization::Release2DIds()
{
	for (size_t i = 0; i < _2D_Line1Ids_N.size();++i)
	{
		for (size_t j = 0; j < _2D_Line1Ids_N[i].size();++j)
		{
			CleanIdBufferForReuse(_2D_Line1Ids_N[i][j]);
		}
		_2D_Line1Ids_N[i].clear();
	}
	for (size_t i = 0; i < _2D_Line1Ids_P.size();++i)
	{
		for (size_t j = 0; j < _2D_Line1Ids_P[i].size();++j)
		{
			CleanIdBufferForReuse(_2D_Line1Ids_P[i][j]);
		}
		_2D_Line1Ids_P[i].clear();
	}
	for (size_t i = 0; i < _2D_Line2Ids_N.size();++i)
	{
		for (size_t j = 0; j < _2D_Line2Ids_N[i].size();++j)
		{
			CleanIdBufferForReuse(_2D_Line2Ids_N[i][j]);
		}
		_2D_Line2Ids_N[i].clear();
	}
	for (size_t i = 0; i < _2D_Line2Ids_P.size();++i)
	{
		for (size_t j = 0; j < _2D_Line2Ids_P[i].size();++j)
		{
			CleanIdBufferForReuse(_2D_Line2Ids_P[i][j]);
		}
		_2D_Line2Ids_P[i].clear();
	}
	for (size_t i = 0; i < _2D_TriIds_N.size();++i)
	{
		CleanIdBufferForReuse(_2D_TriIds_N[i]);
	}
	for (size_t i = 0; i < _2D_TriIds_P.size();++i)
	{
		CleanIdBufferForReuse(_2D_TriIds_P[i]);
	}
	_2D_TriIds_P.clear();
	_2D_TriIds_N.clear();
	_2D_Line1Ids_N.clear();
	_2D_Line1Ids_P.clear();
	_2D_Line2Ids_N.clear();
	_2D_Line2Ids_P.clear();
}