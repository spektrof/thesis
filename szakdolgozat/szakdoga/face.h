#ifndef FACE_H_INCLUDED
#define FACE_H_INCLUDED

//
// Keszitette: Toth Mate
// Terbeli, torottvonallal hatarolt feluleteket (tovabbiakban lapok) reprezentalo sablon tipus.
// Globalis konvenciot kovetve a lapok nem taroljak hatarpontjaikat, csupan hivatkoznak az azokat tarolo vektorra.
// Az egyes pontokat a vektorban elfoglalt helyukkel adjuk meg.

#include <utility>
#include <algorithm>
#include <vector>
#include "vectors.h"
#include "planes.h"
#include "poly2.h"
#include "indexiterator.h"

namespace approx{

	//Tetszoleges, a Vector3 sablonnal hasznalhato skalar tipussal parameterezheto
	template <class T> class Face{
		std::vector< Vector3<T> > *vecs, *normals; //a vektorokat es normalvektorokat tartalmazo tarolok

		std::vector<int> inds; //a hatarpontok indexei
		int normal_id; //a normalvektor indexe

		void calc_normal(){ //amennyiben nem kapott normalvektort inicializalasnal, a megadott pontokbol kiszamolja
			Vector3<T> n = cross(points(2) - points(1), points(0) - points(1));
			n.normalize();
			normal_id = normals->size();
			normals->push_back(n);
		}


		//Az [A,B] el elvagasa a numerikus hiba miatt nem lenne egyenlo a [B,A] sik elvagasaval
		//ez a fuggveny gondoskodik rola hogy ez a problema ne keruljon elo es az azonos oldal
		//azonos sikkal valo vagasa mas lapon is ugyanazt az eredmenyt adja
		Vector3<T> cut_point_stable(int pind1, int pind2, T sign1, T sign2) const {
			if (indicies(pind1) > indicies(pind2)) {
				std::swap(pind1, pind2);
				std::swap(sign1, sign2);
			}
			T   all = (abs(sign1) + abs(sign2)),
				div = abs(sign1 / all),
				div2 =abs(sign2 /all);
			return div2*points(pind1) + div*points(pind2);
		}

	public:

		typedef ConstIndexIterator<Vector3<T>> VertexIterator;

		//a sikkal valo vagas eredmenye
		struct CutResult{
			Face<T> positive, //a sik pozitiv oldalra eso resz
					negative; //a sik negativ oldalra eso resz
			std::vector<int> pt_inds; //az elvalasztovonalra eso pontok indexei
			int points_added; //a hozzaadott pontok szama <= pt_inds.size()
		};

		//konstrukror mely szamolja es beilleszti a normalist
		Face(std::vector< Vector3<T> >* vertices, const std::vector<int>& ids, std::vector< Vector3<T> >* _normals) : vecs(vertices), inds(ids), normals(_normals){ calc_normal(); }
		//konstrukror mely szamolja es beilleszti a normalist, de mozgatast hasznal a megadott pont indexekre
		Face(std::vector< Vector3<T> >* vertices, std::vector<int>&& ids, std::vector< Vector3<T> >* _normals) : vecs(vertices), inds(ids), normals(_normals){ calc_normal(); }
		//megadott normalvektort felhasznalo konstruktor
		Face(std::vector< Vector3<T> >* vertices, const std::vector<int>& ids, std::vector< Vector3<T> >* normals, int n_id) : vecs(vertices), inds(ids), normals(normals), normal_id(n_id){}
		//megadott normalvektort felhasznalo konstruktor, mozgatja a megadott pontindex vektort
		Face(std::vector< Vector3<T> >* vertices, std::vector<int>&& ids, std::vector< Vector3<T> >* normals, int n_id) : vecs(vertices), inds(ids), normals(normals), normal_id(n_id){}
		//megadott de beszurando normalist hasznalo konstruktor
		Face(std::vector< Vector3<T> >* vertices, const std::vector<int>& ids, std::vector< Vector3<T> >* _normals, const Vector3<T>& normal)
			: vecs(vertices), inds(ids), normals(_normals), normal_id(normals->size()){
			normals->push_back(normal);
		}
		//megadott de beszurando normalist hasznalo konstruktor a pontok mozgatasaval
		Face(std::vector< Vector3<T> >* vertices, std::vector<int>&& ids, std::vector< Vector3<T> >* _normals, const Vector3<T>& normal)
			: vecs(vertices), inds(ids), normals(_normals), normal_id(normals->size()){
			normals->push_back(normal);
		}
		//iteratorokkal mukodo konstruktor, az iteratorok az indexeket adjak meg
		template <class Iter> Face(std::vector< Vector3<T> >* vertices, Iter fst,Iter lst, std::vector< Vector3<T> >* _normals, int n_id) : 
			vecs(vertices), inds(fst,lst), normals(_normals), normal_id(n_id) {}

		//masolo konstruktor, linearis a pontok szamaban
		Face(const Face&) = default;
		//mozgato konstruktor konstans idovel
		Face(Face&& f) : vecs(f.vecs), normals(f.normals), inds(std::move(f.inds)), normal_id(f.normal_id){}

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
		Face& operator = (Face&& f){
			inds = std::move(f.inds);
			vecs = f.vecs;
			normals = f.normals;
			normal_id = f.normal_id;
			return *this;
		}

		//egyenloseg teszteles a hatarolo pontok alapjan
		bool operator == (const Face& other) const{
			return size() == other.size() && std::equal(begin(), end(), other.begin());
		}

		//az indexeket tartalmazo vektorra hivatkozas
		const std::vector<int>& indicies() const { return inds; }
		std::vector<int>& indicies(){ return inds; }
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
		Face migrate_to(std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals){
			return Face(target_vecs,std::move(inds),target_normals,normal_id);
		}
		Face migrate_to(std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals) const{
			return Face(target_vecs, inds, target_normals, normal_id);
		}

		//pontok sorrendjenek megforditasa
		void reverse_order(){
			std::reverse(inds.begin(), inds.end());
		}


		//visszaadja a lap ellentett lapjat, megforditott bejarassal es normalvektorral
		//fontos: a normalvektor ellentettjet beszurja a taroloba
		Face reversed() const {
			std::vector<int> tmpind(inds.rbegin(), inds.rend());
			return Face(vecs, std::move(tmpind), normals, normal()*-1);
		}

		//sulypont kiszamolasa
		Vector3<T> center() const {
			Vector3<T> avg;
			for (const Vector3<T>& pt : *this){
				avg += pt;
			}
			avg /= static_cast<T>(size());
			return avg;
		}

		//lekepezes a megadott x es y vektorok altal kifeszitett sikra 2 dimenzios lapkent
		Polygon2<T> to_2d(const Vector3<T>& x, const Vector3<T>& y) const {
			std::vector<Vector2<T>> pts;
			for (const Vector3<T>& v : *this){
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
			T sign1 = p.classify_point(points(0)), sign2;
			int n = size();
			std::vector<int> pos, neg,cut;
			int pts_added=0;
			for (int i = 0; i < n; ++i){
				sign2 = p.classify_point(points((i + 1) % n));
				float sign = sign1*sign2;
				if (sign1 < 0){ //negativ oldalhoz tartozik
					neg.push_back(inds[i]);
					if (sign2 > 0){ //a kovetkezo pont pozitiv, kozottuk vagni kell
						neg.push_back(vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						vecs->push_back(np);
						++pts_added;
					}
				}
				else if (sign1 > 0){ //pozitiv oldalhoz tartozik
					pos.push_back(inds[i]);
					if (sign2 < 0){ //a kovetkezo pont negativ, kozottuk vagni kell
						neg.push_back(vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						vecs->push_back(np);
						++pts_added;
					}
				}
				else{ //a sikon vagyunk
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
		template<class MapType> CutResult cut_by(const Plane<T>& p,MapType& m) const {
			T sign1 = p.classify_point(points(0)), sign2;
			int n = size();
			std::vector<int> pos, neg, cut;
			int pts_added = 0;
			for (int i = 0; i < n; ++i){
				sign2 = p.classify_point(points((i + 1) % n));
				float sign = sign1*sign2;
				if (sign1 < 0){
					neg.push_back(inds[i]);
					if (sign2 > 0){
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						int ind;
						if (!m.count(np)){
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
				else if (sign1 > 0){
					pos.push_back(inds[i]);
					if (sign2 < 0){
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						int ind;
						if (!m.count(np)){
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
				else{
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
		CutResult cut_by(const Plane<T>& p, std::vector<Vector3<T>>* target_vecs, std::vector<Vector3<T>>* target_normals) const{
			
			if (target_vecs == vecs && target_normals == normals) return cut_by(p);

			T sign1 = p.classify_point(points(0)), sign2;
			int n = size();
			std::vector<int> pos, neg,cut;
			int pts_added = 0;
			for (int i = 0; i < n; ++i){
				sign2 = p.classify_point(points((i + 1) % n));
				float sign = sign1*sign2;
				target_vecs->push_back(points(i));
				++pts_added;
				if (sign1 < 0){
					neg.push_back(target_vecs->size() - 1);
					if (sign2 > 0){
						neg.push_back(target_vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						target_vecs->push_back(np);
						++pts_added;
					}
				}
				else if (sign1 > 0){
					pos.push_back(target_vecs->size() - 1);
					if (sign2 < 0){
						neg.push_back(target_vecs->size());
						pos.push_back(neg.back());
						cut.push_back(neg.back());
						Vector3<T> np = cut_point_stable(i, (i + 1) % n, sign1, sign2);
						target_vecs->push_back(np);
						++pts_added;
					}
				}
				else{
					pos.push_back(target_vecs->size()-1);
					neg.push_back(target_vecs->size()-1);
					cut.push_back(target_vecs->size()-1);
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

	};



}




#endif