#ifndef BODY_H_INCLUDED
#define BODY_H_INCLUDED

//
// Keszitette: Toth Mate
// Test tipus mely indexekkel hivatkozik a hatarolo lapjaira, valamint kenyelmi fuggvenyeket nyujt.
// A test kepes meghatarozni terfogatat, kozeppontjat
// 

#include <vector>
#include <algorithm>
#include "face.h"
#include "indexiterator.h"
#include "polygraph.h"

namespace approx{

	//Parametere a skalar tipus ami felett ertelmezzuk a vektorteret
	template <class T> class Body{
	protected:
		std::vector<Face<T>>* _faces;
		std::vector<int> inds;


	public:
		Body(std::vector<Face<T>>* f, const std::vector<int>& i) : _faces(f), inds(i){}
		Body(std::vector<Face<T>>* f, std::vector<int>&& i) : _faces(f), inds(i){}
		Body(const Body&) = default;
		Body(Body&& b) : _faces(b._faces), inds(std::move(b.inds)){}

		typedef T ScalarType;

		typedef IndexIterator<Face<T>> FaceIterator;
		typedef ConstIndexIterator<Face<T>> ConstFaceIterator;

		Body& operator = (const Body& b) = default;
		Body& operator =(Body&& b){
			inds = std::move(b.inds);
			_faces = b._faces;
			return *this;
		}

		//hasznalhato-e a test
		bool valid() const { return _faces && inds.size(); }
		//bool konverzio, erteke a valid metoduseval egyezik
		operator bool() const { return valid(); }

		//mozgatas masik tarolora hivatkozassal
		Body migrate_to(std::vector<Face<T>>* fcs){
			return Body(fcs, std::move(inds));
		}

		//masolas masik tarolora hivatkozassal
		Body migrate_to(std::vector<Face<T>>* fcs) const {
			return Body(fcs, inds);
		}

		//a test lapszama
		int size() const { return inds.size(); }
	    //az i. lap indexe a taroloban
		int indicies(size_t i) const { return inds[i]; }
		//indexlista lekerdezese konstans hozzaferesre
		const std::vector<int>& indicies() const { return inds; }
		//indexlista lekerdezese
		std::vector<int>& indicies() { return inds; }
		
		//az i. lap elerese
		Face<T>& faces(size_t i){ return _faces->operator[](inds[i]); }
		const Face<T>& faces(size_t i) const { return _faces->operator[](inds[i]); }

		FaceIterator begin() { return FaceIterator(_faces, &inds, 0); }
		FaceIterator end() { return FaceIterator(_faces, &inds, inds.size()); }

		ConstFaceIterator begin() const { return ConstFaceIterator(_faces, &inds, 0); }
		ConstFaceIterator end() const { return ConstFaceIterator(_faces, &inds, inds.size()); }

		// a test terfogata
		T volume() const {
			T sum = 0;
			for (const Face<T>& f : *this){
				sum += f.to_2d().area() * dot(f.points(0), f.normal());
			}
			sum /= 3;
			return sum;
		}

		//a test sulypontja a burkolo lapokbol kiszamolva
		Vector3<T> centroid() const{
			int cnt = 0;
			Vector3<T> center;
			for (const Face<T>& f : *this){
				center += f.center();
			}
			center /= size();
			return center;
		}

		//eldonti hogy az adott sik metszi-e a testet
		//pontosan akkor igaz, ha a testnek esik a sik pozitiv es negativ oldalara is pontja
		bool intersects_plane(const Plane<T>& plane) const{
			bool negative=false, positive=false;
			ConstFaceIterator it = begin(), endit = end();
			while (it != endit && (!negative || !positive)){
				typename Face<T>::VertexIterator pt = it->begin(),
												 endpt = it->end();
				while (pt != endpt && (!negative || !positive)){
					T sign = plane.classify_point(*pt);
					negative = negative || sign < 0;
					positive = positive || sign > 0;
					++pt;
				}
				++it;
			}
			return negative && positive;
		}

		//a metszo sik sajat koordinatarendszerbe levetitett lapok
		std::vector<std::pair<Polygon2<T>, bool>> cut_surface(const Plane<T>& plane) const {
			//a pontokat valos numerikus ertekuk szerint hasznalom
			//felmerult a kerdes, miszerint a numerikus hiba okozta egyenloseg teszt nem jelent-e gondot,
			//azonban amennyiben a modell szabalyosan van felepitve es a kozos pontok indexe megegyezik,
			//a processzor valtozatlan adatokon vegzi ugyanazokat a muveleteket, igy az eredmenyek hibaval egyutt megegyeznek
			//a modell helyesseget az eloallito algoritmusra bizom ezert felteszem, hogy fennall
			Graph<T> neighbours;
			//bazisnak hasznalt vektorok melyek a sikkal parhuzamosak igy veluk vett skalarszorzatok hasznalhatok a vetitesnel
			std::pair<Vector3<T>, Vector3<T>> base = plane.ortho2d();
			for (const Face<T>& face : *this) {
				std::vector<Vector3<T>> tmp_normals, tmp_vertices;
				//vegigiteralok a test lapjain es metszem oket a sikkal
				typename Face<T>::CutResult cut = face.cut_by(plane, &tmp_vertices, &tmp_normals);
				//grafot epitek az egyes 2 dimenzios pontokbol
				if (cut.pt_inds.size() > 1) { //van a lapnak lenyomata a sikon, normalis eset 2 pont de nem romlik el egyenes szogeknel sem
					//levetitem a pontokat a sikra
					Vector2<T> pt1(dot(base.first, tmp_vertices[cut.pt_inds.front()]), dot(base.second, tmp_vertices[cut.pt_inds.front()])),
						pt2(dot(base.first, tmp_vertices[cut.pt_inds.back()]), dot(base.second, tmp_vertices[cut.pt_inds.back()]));
					//a grafban szomszedokka teszem oket
					neighbours[pt1].push_back(pt2);
					neighbours[pt2].push_back(pt1);
				}
				//mivel mar nincs szuksegem a sokszogekre eldobom a pontjaikat
			}
			//a neighbours tartalmazza a grafot melyet most sokszogekke kell alakitanunk
			std::vector<Polygon2<T>> polys = get_polys(neighbours);
			//rendezem terulet szerint novekvo sorrendbe es emgnezem hany maikban van benne
			std::vector<std::pair<int, T>> sizes;
			sizes.reserve(polys.size());
			for (int i = 0; i < polys.size(); ++i) {
				sizes.push_back({ i,polys[i].area() });
			}
			std::sort(sizes.begin(), sizes.end(), [](const std::pair<int, T>& a, const std::pair<int, T>& b) { return a.second < b.second; });
			std::vector<std::pair<Polygon2<T>, bool>> result;
			result.reserve(polys.size());
			for (int i = 0; i < polys.size(); ++i) {
				bool pos = true;
				for (int j = i + 1; j < polys.size(); ++j) {
					if (polys[sizes[j].first].contains(polys[sizes[i].first])) pos = !pos;
				}
				result.emplace_back(polys[sizes[i].first], pos);
			}
			return result;
		}

		//a legnagyobb atmero hosszaval es iranyaval megegyezo vektor
		Vector3<T> diameter() const {
			Vector3<T> diam;
			for (int i = 0; i < size(); ++i) {
				for (int j = i; j < size(); ++j) {
					for (const Vector3<T>& p1 : faces(i)) {
						for (const Vector3<T>& p2 : faces(j)) {
							Vecotr3<T> d = (p1 - p2);
							if (d.length() > diam.length()) {
								diam = d;
							}
						}
					}
				}
			}
			return diam;
		}


	};

}

#endif