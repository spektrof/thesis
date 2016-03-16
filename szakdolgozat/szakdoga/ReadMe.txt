/////////////////////////////////////////////////////////////////////////////

Hasznalati informacio:
	A glm hasznalatahoz a T: meghajtot az OGLPack-ot hasznalo konyvtarba kell allitani
TODO:
	-textura koordinata kerdesek
	-esetleges cw ccw megoldas
	-approximacios test: csinal szamitas elott
	 egy garbage collectiont, ami viszont nem funkcional ha van befejezetlen vagas
	-jobbkezes balkezes koordinata rendszer
===========================================================================================
                                      EGYESITESHEZ TUTORIAL:
===========================================================================================

A C++ fajlok kozul csak a headerek kellenek a mukodeshez, a tobbi csak random tesztkod.
A forditasnal a glm/glm.hpp-t latnia kell a forditonak.

Alabb talalhato egy obj fajl mely egy kockat tartalmaz, ez lesz a kozelitendo test.
A peldakod ezt hasznalja fel. A peldakod a konzolra irat dolgokat, igyekszik a lenyegre torekedni

===========================================================================================
test.obj fajl tartalma:
===========================================================================================


# Created with Anim8or 0.98
# Object "object01":

mtllib test.mtl

# Shape "cube01":
g cube01
# No. points 8:
v 12.0443 13.9548 15.0123
v 12.0443 13.9548 37.0387
v 12.0443 38.3307 15.0123
v 12.0443 38.3307 37.0387
v 34.0707 13.9548 15.0123
v 34.0707 13.9548 37.0387
v 34.0707 38.3307 15.0123
v 34.0707 38.3307 37.0387

# No. normals 30:
vn 0 0 -1
vn 0 0 1
vn -1 0 0
vn 1 0 0
vn 0 1 0
vn 0 -1 0
vn 0 0 -1
vn 0 -1 0
vn -1 0 0
vn 0 0 1
vn 0 -1 0
vn -1 0 0
vn 0 0 -1
vn 0 1 0
vn -1 0 0
vn 0 0 1
vn 0 1 0
vn -1 0 0
vn 0 0 -1
vn 0 -1 0
vn 1 0 0
vn 0 0 1
vn 0 -1 0
vn 1 0 0
vn 0 0 -1
vn 0 1 0
vn 1 0 0
vn 0 0 1
vn 0 1 0
vn 1 0 0

# No. texture coordinates 8:
vt 0 0
vt 0 0
vt 0 1
vt 0 1
vt 1 0
vt 1 0
vt 1 1
vt 1 1

# No. faces 6:

usemtl ___default___

f 3/3/13 7/7/25 5/5/19 1/1/7   
f 6/6/22 8/8/28 4/4/16 2/2/10
f 2/2/12 4/4/18 3/3/15 1/1/9
f 7/7/27 8/8/30 6/6/24 5/5/21
f 4/4/17 8/8/29 7/7/26 3/3/14
f 5/5/20 6/6/23 2/2/11 1/1/8

# End of Object "object01"

==================================================================================================
 C++ kod pelda
==================================================================================================
	//Ez az egyetlen include ami kell a mukodteteshez
	#include "approximator.h"

	//...

	//ez az osztaly tartja szamon az approximaciot
	approx::Approximator<float> app;

	//a megadott fajlnevben levo test a celtest, a kezdo kocka atom 0.5-os kerettel veszi korbe
	if (!app.set_target("test.obj", 0.5f)) {
		cout << "HIBA A FAJL BETOLTESENEL!\n";
	}

	//A celtest az app.target().body()-ban erheto el, a body.h-ban bovebb info talalhato rola
	//es a metodusairol
	cout << "A celtest terfogata: " << app.target().body().volume() << "\n";

	//vagosik
	approx::Plane<float> p({ 1,0,0 }, 15.0f);

	//az app.container() tartalmazza az atomokat es nyujt lehetoseget az approximacios muveletekre
	//a 0. atomot elvagom az elobb megadott sikkal
	//a cut egy vagasi eredmeny, a vagast vegrehajtom, de az atomok nem kerulnek be a taroloba
	Approximation<float>::CutResult cut = app.container().cut(0, p);
	
	//mindenfele ugyes okos vizsgalatok a keletkezett atomokon hogy jok-e, pl terfogat ellenorzes
	//a metszet terfogat meg bugol, de a sima terfogat jol mukodik
	//a pozitiv es negativ oldal a metszosikhoz kepesti elhelyezkedest jelenti 
	cout << "negativ oldali keletkezett atom terfogata: " << cut.negative()->volume() << "\n";
	cout << "pozitiv oldali keletkezett atom terfogata: " << cut.positive()->volume() << "\n";

	//mondjuk hogy tetszik az eredmeny, berakjuk az atomokat
	cut.choose_both();
	//mostmar ket atomunk van
	//choose_negative() - choose_positive() ezek csak a negativ vagy csak a pozitiv oldali atomot hagyjak meg
	//ha nem tetszene az eredmeny akkor cut.undo();
	//az undo csak egyszer mukodik, es ha mar elfogadtuk choose_*-al akkor nem undozhatjuk

	//az app.container().atoms(i) az i. atom kozvetlen konstans eleresre, pl teszteli hogy az adott sik atmegy-e rajta
	approx::Plane<float> p2({ 1,0,0 }, 16.0f);
	if (app.container().atoms(1).intersects_plane(p2)) {
		cout << "az 1. indexu atomon atmegy a p2 sik, vagok ezzel is\n";
		app.container().cut(1, p2).choose_negative();
	}

	//lekerem az atomok rajzolasi adatait
	approx::BodyList data = app.atom_drawinfo();
	//data.points - vertex adatok
	//data.indicies - omlesztve az osszes index
	//data.index_ranges index hatarok:
	//az [index_ranges[i], indes_ranges[i+1]) intervallum az indexekbol egy atom
	//tehat az i. atomnal index_ranges[i] az elso es van index_ranges[i+1]-index_ranges[i] darab
	//GL_TRIANGLES modban mukodnie kell
	for (int i = 0; i < data.index_ranges.size() - 1; ++i) {
		cout << " -------- Atom" << i << " -------- \n";
		for (int j = data.index_ranges[i]; j < data.index_ranges[i + 1]; ++j) {
			cout << data.points[ data.indicies[j] ].x << ", "
				 << data.points[ data.indicies[j] ].y << ", "
				 << data.points[ data.indicies[j] ].z << "\n";
		}
	}

	//hasonlo modon kerheto el a rajzolando celtest is
	approx::BodyList targetdata = app.target_drawinfo();

	//elmentem obj fajlformatumba az approximalt testet
	ObjectWriter<float>::save_obj("approx.obj", app.container().approximated_body());

	//ha az approximaciot ujra akarjuk kezdeni akkor
	app.restart();
	//ez utan ismet egyetlen kezdo kocka atomunk lesz
	//vigyazat, ervenytelenit minden korabbi esetleges referenciat, atom vagy body eredmenyt melyet elkertunk


