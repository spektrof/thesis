#ifndef APPROXIMATOR_H_INCLUDED
#define APPROXIMATOR_H_INCLUDED

//
// Keszitette Toth Mate
// Az approximacios folyamat lebonyolitasat vegzo osztaly mely a felhasznalo szamara biztosit kenyelmes hozzaferest.
//

#include <string>
#include <memory>
#include <algorithm>
#include "targetbody.h"
#include "approximation.h"

#ifndef APPROX_NO_CONVERSION
#include "conversion.h"
#endif 

#include "../Utils/objio.h"
#include "diffvec.h"

namespace approx {

	template <class T> class Approximator {
		std::unique_ptr<Approximation<T>> app;
		std::unique_ptr<TargetBody<T>> tb;
		T border;

	public:
		//default konstruktor, az approximalo meg nem all keszen a hasznalatra
		Approximator() {}
		//az adott fajl celtestnek hasznalasa, adott távolságú hatarolo kockaval indulva
		Approximator(const std::string& filename,T _border) {
			set_target(filename,_border);
		}

		//a megadott celtest felhasznalasa, adott tavolsagu hatarolo kockaval indulva
		Approximator(std::unique_ptr<TargetBody<T>>&& target,T _border) {
			set_target(std::move(target),_border);
		}

		//pontosan akkor igaz, ha az approximacios munkafolyamat megkezdheto
		bool valid() const {
			return app && tb;
		}

		//az approximacios atom tarolo
		Approximation<T>& container() {
			return *app;
		}

		//celtest
		const TargetBody<T>& target() const {
			return *tb;
		}

		//a jelenlegi cel approximalasanak elorol kezdese
		//pontosan akkor ad igazat, ha a folyamat megkezdheto
		bool restart() {
			if (!(tb && tb->body().size())) return false;
			//app = std::make_unique<Approximation<T>>(tb.get(), border); //C++14tol
			app = std::unique_ptr<Approximation<T>>(new Approximation<T>(tb.get(), border));
			return true;
		}
		
		//================================================================================
		//Kezdes, beallitas
		//================================================================================

		//A megadott fajlbol akarunk betolteni.
		//A masodik parameter a korulvevo befogo kocka lapjainak tavolsaga a testtol,
		//a harmadik opcionalis parameter arra valo, ha betoltesnel korrekciot kivanunk vegezni a testen.
		//Amennyiben a negyedik parameternek pozitiv erteke van, az approximalt testet egy origo kozepu,
		//adott elhosszusagu kockaba transzformalja
		bool set_target(const std::string& filename, T _border,T merging_epsilon=0, T cube = -1, bool triangulate=true) {
			border = _border;
			//tb = std::make_unique<TargetBody<T>>(); //C++14tol
			tb = std::unique_ptr<TargetBody<T>>(new TargetBody<T>());
			if (!(ObjectLoader<T>::load_obj(filename, *tb, merging_epsilon,triangulate) && tb->body().size())) {
				tb.release();
				app.release();
				return false;
			}
			if (cube > 0) {
				tb->transform_to_origo(cube);
			}
			//app = std::make_unique<Approximation<T>>(tb.get(),border);
			app = std::unique_ptr<Approximation<T>>(new Approximation<T>(tb.get(), border));
			return true;
		}

		//adott celtestet, adott tavolsagu befogo kockaval kezve akarunk kozeliteni
		//Amennyiben a harmadik parameternek pozitiv erteke van, az approximalt testet egy origo kozepu,
		//adott elhosszusagu kockaba transzformalja
		bool set_target(std::unique_ptr<TargetBody<T>>&& target,T _border, T cube = -1) {
			border = _border;
			tb = std::move(target);
			if (!(tb && tb->body().size())) {
				tb.release();
				app.release();
				return false;
			}
			if (cube > 0) {
				tb->transform_to_origo(cube);
			}
			//app = std::make_unique<Approximation<T>>(tb.get(), border); //C++14tol
			app = std::unique_ptr<Approximation<T>>(new Approximation<T>(tb.get(), border));
			return true;
		}

		//================================================================================
		//Fajlba menteshez
		//================================================================================

		//.obj formatumba elmenti az atomokat, egy atom egy csoportket jelenik meg a fajlban
		void save_atoms(const std::string& targetfile) const {
			ObjectWriter<T>::save_obj(targetfile,*app);
		}

		//.obj formatumba elmenti az atomokbol letrejovo approximacios testet
		void save_approximated_body(const std::string& targetfile) const {
			ObjectWriter<T>::save_obj(targetfile, app->approximated_body());
		}

		//================================================================================
		//Rajzolashoz
		//================================================================================

#ifndef APPROX_NO_CONVERSION

		//az osszes atom vertex adata rajzolasra kesz formatumban
		BodyList atom_drawinfo() const {
			return drawinfo<Approximation<T>::ConstIterator, T>(app->begin(), app->end());
		}

		//a celtest rajzolhato vertex adatai
		BodyList target_drawinfo() const {
			return drawinfo(tb->body());
		}

		//az atomokbol eloallo approximacios test vertex adatai rajzolashoz
		BodyList approx_drawinfo(InsideHandling mode = InsideHandling::LeaveOut,T fouriermin=0.5f) const {
			return compact_drawinfo(app->approximated_body(mode,fouriermin));
		}

		//a ket meg be nem illesztett atom rajz adatai
		//elso a negativ oldali masodik a pozitiv oldali
		BodyList cut_drawinfo() const {
			BodyList b1 = compact_drawinfo(*app->last_cut_result().negative());
			BodyList b2 = compact_drawinfo(*app->last_cut_result().positive());
			int n = b1.points.size();
			b1.points.insert(b1.points.end(), b2.points.begin(), b2.points.end());
			for (int x : b2.indicies) {
				b1.indicies.push_back(x+n);
			}
			b1.index_ranges.push_back(b1.indicies.size());
			return b1;
		}

		//az adott indexu atom lapjainak es metszetvetuleteinek a kirajzoltatasra hasznalhato vektora
		std::vector<PolyFace2D> atom2dfaces(int ind) const {
			return drawinfo2d(app->atoms(ind));
		}
#endif
	};

}

#endif
