#ifndef FACE_H_INCLUDED
#define FACE_H_INCLUDED

//
// Keszitette: Toth Mate
// Terbeli, egyetlen, nem onmetszo torottvonallal hatarolt feluleteket (tovabbiakban lapok) reprezentalo sablon tipus.
// Globalis konvenciot kovetve a lapok nem taroljak hatarpontjaikat, csupan hivatkoznak az azokat tarolo vektorra.
// Az egyes pontokat a vektorban elfoglalt helyukkel adjuk meg.

#include <utility>
#include <cmath>
#include <algorithm>
#include <vector>
#include <set>
#include "vectors.h"
#include "planes.h"
#include "poly2.h"
#include "indexiterator.h"

namespace approx{

	//Tetszoleges, a Vector3 sablonnal hasznalhato skalar tipussal parameterezheto
	template <class T> class Face {
		std::vector< Vector3<T> > *vecs, *normals; //a vektorokat es normalvektorokat tartalmazo tarolok

		std::vector<int> inds; //a hatarpontok indexei
		int normal_id; //a normalvektor indexe

		static constexpr float normaleps() { return 0.9999f; }
		void calc_normal(bool ccw) { //amennyiben nem kapott normalvektort inicializalasnal, a megadott pontokbol kiszamolja
			Vector3<T> n = cross(points(2) - points(1), points(0) - points(1));
			n.normalize();
			normal_id = normals->size();
			if (ccw) {
				normals->push_back(n);
			}
			else {
				normals->push_back(-n);
			}
		}


		//Az [A,B] el elvagasa a numerikus hiba miatt nem mindig egyenlo a [B,A] sik elvagasaval
		//ez a fuggveny gondoskodik rola hogy ez a problema ne keruljon elo es az azonos oldal
		//azonos sikkal valo vagasa mas lapon is ugyanazt az eredmenyt adja
		Vector3<T> cut_point_stable(int pind1, int pind2, T sign1, T sign2) const {
			if(sign1 > 0){
				std::swap(pind1, pind2);
				std::swap(sign1, sign2);
			}
			T   all = (std::abs(sign1) + std::abs(sign2)),
				div = std::abs(sign1 / all),
				div2 = std::abs(sign2 / all);
			return div2*points(pind1) + div*points(pind2);
		}

	public:

		typedef ConstIndexIterator<Vector3<T>> VertexIterator;

		//a sikkal valo vagas eredmenye
		struct CutResult {
			Face<T> positive, //a sik pozitiv oldalra eso resz
				    negative; //a sik negativ oldalra eso resz
			std::vector<int> pt_inds; //az elvalasztovonalra eso pontok indexei
			int points_added; //a hozzaadott pontok szama <= pt_inds.size()
		};

		//konstrukror mely szamolja es beilleszti a normalist
		Face(std::vector< Vector3<T> >* vertices, const std::vector<int>& ids, std::vector< Vector3<T> >* _normals, bool ccw = true) :
			vecs(vertices), normals(_normals), inds(ids) {
			calc_normal(ccw);
		}
		//konstrukror mely szamolja es beilleszti a normalist, de mozgatast hasznal a megadott pont indexekre
		Face(std::vector< Vector3<T> >* vertices, std::vector<int>&& ids, std::vector< Vector3<T> >* _normals, bool ccw = true)
			: vecs(vertices), normals(_normals), inds(ids) { calc_normal(ccw); }
		//megadott normalvektort felhasznalo konstruktor
		Face(std::vector< Vector3<T> >* vertices, const std::vector<int>& ids, std::vector< Vector3<T> >* normals, int n_id)
			: vecs(vertices), normals(normals), inds(ids), normal_id(n_id) {}
		//megadott normalvektort felhasznalo konstruktor, mozgatja a megadott pontindex vektort
		Face(std::vector< Vector3<T> >* vertices, std::vector<int>&& ids, std::vector< Vector3<T> >* normals, int n_id)
			: vecs(vertices), normals(normals), inds(ids), normal_id(n_id) {}
		//megadott de beszurando normalist hasznalo konstruktor
		Face(std::vector< Vector3<T> >* vertices, const std::vector<int>& ids, std::vector< Vector3<T> >* _normals, const Vector3<T>& normal)
			: vecs(vertices), normals(_normals), inds(ids), normal_id(normals->size()) {
			normals->push_back(normal);
		}
		//megadott de beszurando normalist hasznalo konstruktor a pontok mozgatasaval
		Face(std::vector< Vector3<T> >* vertices, std::vector<int>&& ids, std::vector< Vector3<T> >* _normals, const Vector3<T>& normal)
			: vecs(vertices), normals(_normals), inds(ids), normal_id(normals->size()) {
			normals->push_back(normal);
		}
		//iteratorokkal mukodo konstruktor, az iteratorok az indexeket adjak meg
		template <class Iter> Face(std::vector< Vector3<T> >* vertices, Iter fst, Iter lst, std::vector< Vector3<T> >* _normals, int n_id) :
			vecs(vertices), normals(_normals), inds(fst, lst), normal_id(n_id) {}

		//masolo konstruktor, linearis a pontok szamaban
		Face(const Face&) = default;
		//mozgato konstruktor konstans idovel
		Face(Face&& f) : vecs(f.vecs), normals(f.normals), inds(std::move(f.inds)), normal_id(f.normal_id) {}

		//masolo ertekadas, default, linearis idovel
		//Face& operator = (const Face& f) = default;
		Face& operator = (const Face& f) {
			inds = f.inds;
			vecs = f.vecs;
			normals = f.normals;
			normal_id = f.normal_id;
			return *this;
		}
		//mozgato ertekadas konstans idoben
		Face& operator = (Face&& f) {
			inds = std::move(f.inds);
			vecs = f.vecs;
			normals = f.normals;
			normal_id = f.normal_id;
			return *this;
		}

		//egyenloseg teszteles a hatarolo pontok alapjan
		bool operator == (const Face& other) const {
			return size() == other.size() && std::equal(begin(), end(), other.begin());
		}

		//az indexeket tartalmazo vektorra hivatkozas
		const std::vector<int>& indicies() const { return inds; }
		std::vector<int>& indicies() { return inds; }
		//az i. pont indexe
		int indicies(size_t i) const { return inds[i]; }
		//a normalvektor indexe
		int normal_index() const { return normal_id; }
		int& normal_index() { return normal_id; }
		//a pontok szama
		size_t size() const { return inds.size(); }

		//a pontokat valojavan tartalamzo tarolok
		std::vector< Vector3<T> >* vertex_container() const { return vecs; }
		std::vector< Vector3<T> >* normal_container() const { return normals; }

		//normalvektor es index alapjan pontok elerese konstans esetre
		const Vector3<T>& normal() const { return normals->operator[](normal_id); }
		const Vector3<T>& points(size_t ind) const { return vecs->operator[](inds[ind]); }

		//a sik kinyerese melyen a lap fekszik
		Plane<T> to_plane() const { return Plane<T>(normal(), points(0)); }

		//iteralhatosaghoz konstans iterator
		VertexIterator begin() const { return VertexIterator(vecs, &inds, 0); }
		VertexIterator end() const { return VertexIterator(vecs, &inds, inds.size()); }

		//a parameterkent megadott tarolokra azonos indexekkel hivatkozo lap elkeszitese amennyiben a ponttarolok kozott masolunk
		Face migrate_to(std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals) {
			return Face(target_vecs, std::move(inds), target_normals, normal_id);
		}
		Face migrate_to(std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals) const {
			return Face(target_vecs, inds, target_normals, normal_id);
		}

		//pontok sorrendjenek megforditasa
		void reverse_order() {
			std::reverse(inds.begin(), inds.end());
		}


		//visszaadja a lap ellentett lapjat, megforditott bejarassal es normalvektorral
		//fontos: a normalvektor ellentettjet beszurja a taroloba
		Face reversed() const {
			std::vector<int> tmpind(inds.rbegin(), inds.rend());
			return Face(vecs, std::move(tmpind), normals, -normal());
		}

		//sulypont kiszamolasa
		Vector3<T> centroid() const {
			//Vector3<T> avg;
			//for (const Vector3<T>& pt : *this){
			//	avg += pt;
			//}
			//avg /= static_cast<T>(size());
			//return avg;
			Vector2<T> pt = to_2d().centroid();
			std::pair<Vector3<T>, Vector3<T>> base = to_plane().ortho2d();
			return pt.x*base.first + pt.y * base.second + to_plane().example_point();
		}

		//lekepezes a megadott x es y vektorok altal kifeszitett sikra 2 dimenzios lapkent
		Polygon2<T> to_2d(const Vector3<T>& x, const Vector3<T>& y) const {
			std::vector<Vector2<T>> pts;
			for (const Vector3<T>& v : *this) {
				pts.push_back({ dot(v, x), dot(v, y) });
			}
			return Polygon2<T>(std::move(pts));
		}

		//lekepezes ketto dimenzioba a lapra melyen fekszik
		//a feszito koordinata:
		// x = elso es masodik hatarpont kozotti vektor
		// y = cross(normal,x)
		Polygon2<T> to_2d() const {
			//Vector3<T> y = (points(1) - points(0)).normalized();
			//Vector3<T> x = cross(get_normal(), y).normalized();
			if (!size()) return Polygon2<T>({});
			std::pair<Vector3<T>, Vector3<T>> base = to_plane().ortho2d();
			return to_2d(base.first, base.second);
		}

		//elvagas a parameterkent megadott sikkal, a vagasnal keletkezo uj pontokat a sajat tarolo vektoraba szurja
		//a keletkezo lapok ugyanarra a tarolora hivatkoznak
		//a vagas feltetelezi, hogy a lap konvex
		//a muveletigeny linearis a csucspontok szamaban
		CutResult cut_by(const Plane<T>& p) const {
			if (std::abs(dot(p.normal(),normal()))>=normaleps() ) return coplanar_cut(p);


			T sign1 = p.classify_point(points(0)), sign2;
			int n = size();
			std::vector<int> pos, neg, cut;
			int pts_added = 0;
			for (int i = 0; i < n; ++i) {
				sign2 = p.classify_point(points((i + 1) % n));
				if (sign1 < 0) { //negativ oldalhoz tartozik
					neg.push_back(inds[i]);
					if (sign2 > 0) { //a kovetkezo pont pozitiv, kozottuk vagni kell
						neg.push_back(vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						vecs->push_back(np);
						++pts_added;
					}
				}
				else if (sign1 > 0) { //pozitiv oldalhoz tartozik
					pos.push_back(inds[i]);
					if (sign2 < 0) { //a kovetkezo pont negativ, kozottuk vagni kell
						neg.push_back(vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						vecs->push_back(np);
						++pts_added;
					}
				}
				else { //a sikon vagyunk
					pos.push_back(inds[i]);
					neg.push_back(inds[i]);
					cut.push_back(inds[i]);
				}
				sign1 = sign2;
			}
			return{ Face<T>(vecs, std::move(pos), normals, normal_id),
					Face<T>(vecs, std::move(neg), normals, normal_id),
					cut,
					pts_added };
		}

		//a sajatjaval megegyezo taroloba vagas
		//masodik paramtere egy Vector3<T> -> int kepezo asszociativ tarolo, a kovetkezokkel kell rendelkeznie:
		// count(Vector3<T>) metodus, mely 0 ha a pont nem szerepel benne, pozitiv kulonben
		// [] operator : Vector3<T> -> int, megadja a ponthoz tarsitott indexet
		// pelda tipus: std::map<Vector3<T>,int>,std::unordered_map<Vector3<T>,int> a szukseges rendezessel vagy hasitassal
		//a muveletigeny linearis a pontok szamaban, valamint a vagopontok beszurasanal a megadott Maptype kereso es beszuro koltsege
		template<class MapType> CutResult cut_by(const Plane<T>& p, MapType& m) const {

			if (std::abs(dot(p.normal(), normal())) >= normaleps() ) return coplanar_cut(p);
			T sign1 = p.classify_point(points(0)), sign2;
			int n = size();
			std::vector<int> pos, neg, cut;
			int pts_added = 0;
			for (int i = 0; i < n; ++i) {
				sign2 = p.classify_point(points((i + 1) % n));
				if (sign1 < 0) {
					neg.push_back(inds[i]);
					if (sign2 > 0) {
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						int ind;
						if (!m.count(np)) {
							ind = vecs->size();
							vecs->push_back(np);
							m[np] = ind;
							++pts_added;
						}
						else ind = m[np];
						neg.push_back(ind);
						pos.push_back(ind);
						cut.push_back(ind);

					}
				}
				else if (sign1 > 0) {
					pos.push_back(inds[i]);
					if (sign2 < 0) {
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						int ind;
						if (!m.count(np)) {
							ind = vecs->size();
							vecs->push_back(np);
							m[np] = ind;
							++pts_added;
						}
						else ind = m[np];
						neg.push_back(ind);
						pos.push_back(ind);
						cut.push_back(ind);
					}
				}
				else {
					pos.push_back(inds[i]);
					neg.push_back(inds[i]);
					cut.push_back(inds[i]);
				}
				sign1 = sign2;
			}
			return{ Face<T>(vecs, std::move(pos), normals, normal_id),
				Face<T>(vecs, std::move(neg), normals, normal_id),
				cut,
				pts_added };
		}

		//elvagas teljesen uj taroloba az adott sikkal
		CutResult cut_by(const Plane<T>& p, std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals) const {

			if (target_vecs == vecs && target_normals == normals) return cut_by(p);

			if (std::abs(dot(p.normal(), normal())) >= normaleps()) return coplanar_cut(p,target_vecs,target_normals);

			T sign1 = p.classify_point(points(0)), sign2;
			int n = size();
			std::vector<int> pos, neg, cut;
			int pts_added = 0;
			for (int i = 0; i < n; ++i) {
				sign2 = p.classify_point(points((i + 1) % n));
				target_vecs->push_back(points(i));
				++pts_added;
				if (sign1 < 0) {
					neg.push_back(target_vecs->size() - 1);
					if (sign2 > 0) {
						neg.push_back(target_vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						target_vecs->push_back(np);
						++pts_added;
					}
				}
				else if (sign1 > 0) {
					pos.push_back(target_vecs->size() - 1);
					if (sign2 < 0) {
						neg.push_back(target_vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						target_vecs->push_back(np);
						++pts_added;
					}
				}
				else {
					pos.push_back(target_vecs->size() - 1);
					neg.push_back(target_vecs->size() - 1);
					cut.push_back(target_vecs->size() - 1);
				}
				sign1 = sign2;
			}
			target_normals->push_back(normal());
			int n_id = target_normals->size() - 1;
			return{ Face<T>(target_vecs, std::move(pos), target_normals, n_id),
					Face<T>(target_vecs, std::move(neg), target_normals, n_id),
					cut,
					pts_added };
		}

		//pontosan akkor igaz ha CCW koruljarasi iranyban vannak felsorolva a pontjai
		bool is_ccw() const {
			bool cc1 = to_2d().is_ccw();
			std::pair<Vector3<T>, Vector3<T>> base = to_plane().ortho2d();
			bool cc2 = dot(cross(base.first, base.second), normal()) > 0;
			return cc1 == cc2;
		}

		//ha van a es b indexu pontja, beszurja kozejuk az ind indexut,
		//ha nincs ilyen, nem tesz semmit
		//visszatérési erteke a beszuras sikeressege
		bool insert_index(int a, int b, int ind) {

			if ((inds.front() == a && inds.back() == b) || (inds.front() == b && inds.back() == a)) {
				inds.push_back(ind);
				return true;
			}
			for (int i = 1; i < size(); ++i) {
				if ((inds[i] == a && inds[i - 1] == b) || (inds[i] == b && inds[i - 1] == a)) {
					inds.insert(inds.begin() + i, ind);
					return true;
				}
			}
			return false;
		}

		//megkeresi az adott tarolo beli indexu pont szomszedait a sokszogben, ha megtalalta, a tarolobeli indexeiket adja meg
		//ha nem talalhato ilyen pont, a visszateresi erteke a {-1,-1} paros
		std::pair<int,int> neighbours_of(int real_ind) const {
			int i = 0;
			int n = size();
			while (i < n && inds[i] != real_ind) ++i;
			if (i < n) {
				return { inds[(i+1)%n], inds[(i+n-1)%n] };
			}
			else {
				return { -1, -1 };
			}
		}

		private:
			//a koplanaris vagas specialis esetet igenyel a numerikus hibak miatt
			CutResult coplanar_cut(const Plane<T>& p) const {
				bool zero = false;
				bool neg = false;
				bool pos = false;
				for (int i = 0; i < size() && !zero; ++i) {
					T clf = p.classify_point(points(i));
					neg = neg || clf < 0;
					pos = pos || clf > 0;
					zero = clf == 0 || (neg && pos);
				}
				if (zero) { //pontosan a sikon vagyunk
					return{ *this, *this, indicies(), 0 };
				}
				else if (pos) {
					return{ *this, Face<T>(vecs,std::vector<int>(),normals,normal_id), std::vector<int>(), 0 };
				}
				else {
					return{ Face<T>(vecs,std::vector<int>(),normals,normal_id), *this, std::vector<int>(), 0 };
				}
			}

			CutResult coplanar_cut(const Plane<T>& p, std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals) const {
				bool zero = false;
				bool neg = false;
				bool pos = false;
				std::vector<int> tmpind;
				target_normals->push_back(normal());
				for (int i = 0; i < size(); ++i) {
					tmpind.push_back(target_vecs->size());
					target_vecs->push_back(points(i));
					T clf = p.classify_point(points(i));
					neg = neg || clf < 0;
					pos = pos || clf > 0;
					zero = zero || clf == 0 || (neg && pos);
				}
				int n_id = target_normals->size() - 1;
				if (zero) { //pontosan a sikon vagyunk
					return{ Face<T>(target_vecs,tmpind,target_normals, n_id),
						Face<T>(target_vecs,tmpind,target_normals, n_id),
						tmpind,
						(int)tmpind.size() };
				}
				else if (pos) {
					return{ Face<T>(target_vecs,tmpind,target_normals,n_id),
						Face<T>(target_vecs,std::vector<int>(),target_normals, n_id),
						std::vector<int>(),
						(int)tmpind.size() };
				}
				else {
					return{ Face<T>(target_vecs,std::vector<int>(),target_normals, n_id),
						Face<T>(target_vecs,tmpind,target_normals,n_id),
						std::vector<int>(),
						(int)tmpind.size() };
				}
			}
	};



}




#endif