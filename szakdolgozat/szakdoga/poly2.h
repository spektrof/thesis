#ifndef POLY2_H_INCLUDED
#define POLY2_H_INCLUDED

//
// Keszitette Toth Mate
// Kettodimenzios sokszog sablon. A haromdimenzios lappal ellentetben minden pontjat helyben tarolja.
// Feladata a teruletenek kiszamitasa, valamint a vagas implementalasa.
//

#include <vector>
#include <cmath>
#include <algorithm>
#include "vectors.h"
#include "planes.h"

namespace approx{

	// Sokszog tipus 2 dimenzioban.
	// Tetszoleges skalar tipussal paramezerezheto ami megfelel a Vector2 elvarasainak,
	// valamint elvegezheto rajta az "x > 0" es az abszolutertek muvelet.
	template<class T> class Polygon2{
		std::vector<Vector2<T>> pts;
		typedef typename std::vector<Vector2<T>>::const_iterator ConstIterator;
		typedef typename std::vector<Vector2<T>>::iterator Iterator;
	public:
		//vektorral megadott konstruktorok
		Polygon2(const std::vector<Vector2<T>>& p) : pts(p) {}
		Polygon2(std::vector<Vector2<T>>&& p) : pts(p){}
		//masolo, mozgato valamint iteratorbol masolo konstruktorok
		Polygon2(const Polygon2&) = default;
		Polygon2(Polygon2&& p) : pts(std::move(p.pts)){}
		template<class Iter> Polygon2(Iter beg, Iter end) : pts(beg, end){}

		//ertekadas masolasra es mozgatasra
		Polygon2& operator = (const Polygon2&) = default;
		Polygon2& operator = (Polygon2&&) = default;

		//csak olvashato iteratorok a pontok bejarasara
		ConstIterator begin() const { return pts.cbegin(); }
		ConstIterator end() const { return pts.cend(); }
		//irhato es olvashato iteratorok a pontokon
		Iterator begin() { return pts.begin(); }
		Iterator end() { return pts.end(); }

		bool operator == (const Polygon2<T>& p) const {
			return pts == p.pts;
		}

		//pontok szama
		int size() const { return pts.size(); }
		//pontokat tartalmazo vektor konstans elerese
		const std::vector<Vector2<T>>& points() const { return pts; }
		//i. pont elerese
		const Vector2<T>& points(size_t i) const { return pts[i]; }
		Vector2<T>& points(size_t i) { return pts[i]; }
		
		//elojeles terulet, elojele seggithet annak eldonteseben hogy cw vagy ccw felsorolasban van megadva
		T signed_area() const {
			int n = pts.size();
			T result = 0;
			for (int i = 0; i < n; ++i){
				result += pts[(i + 1) % n].x*pts[i].y - pts[i].x*pts[(i + 1) % n].y;
			}
			return result / static_cast<T>(2);
		}

		//teruletet kiszamito metodus, a pontossaga a megadott skalartol fugg
		//kepes kezelni CW es CCW sorrendben megadott alakzatot is
		T area() const {
			return abs(signed_area());
		}

		bool is_ccw() const {
			return signed_area() < 0;
		}

		//vagasi eredmeny tipus
		struct CutResult{
			Polygon2 negative, //a vago egyenes negativ oldalara eso resz
					 positive; //a vago egyenes pozitiv oldalara eso resz
		};

		//a megadott vonallal elvagva kapott sokszogek szamitasa
		//feltetelezzuk, hogy a sokszog konvex, ebbol kovetkezoen az eredmeny is az
		//muveletigeny linearis a csucspontok szamaban
		CutResult cut_by(const Line<T> l) const {
			std::vector<Vector2<T>> pos, neg;
			T sign1 = l.classify_point(pts[0]); //az aktualisan vizsgalt pont elhelyezkedese a sikhoz kepest
			const int n = size();
			for (int i = 0; i < n;++i){ //vegigiteralok a pontok kozott
				T sign2 = l.classify_point(pts[(i + 1) % n]); //a kovetkezo pont betajolasa
				if (sign1 < 0){//a pont a negativ oldalra esik
					neg.push_back(pts[i]);
					if (sign2 > 0){//a kovetkezo pont pozitiv, a ketto kozott vagas keletkezik
						T div = abs(sign1 / (abs(sign1)+abs(sign2)));
						Vector2<T> cpt = (1 - div)*pts[i] + div*pts[(i + 1) % n];
						pos.push_back(cpt);
						neg.push_back(cpt);
					}
				}
				else if (sign1 > 0){//a pont a pozitiv oldalra esik
					pos.push_back(pts[i]);
					if (sign2 < 0){//a kovetkezo pont negativ, a ketto kozott vagas keletkezik
						T div = abs(sign1 / (abs(sign1) + abs(sign2)));
						Vector2<T> cpt = (1 - div)*pts[i] + div*pts[(i + 1) % n];
						pos.push_back(cpt);
						neg.push_back(cpt);
					}
				}
				else{ //a pont a vonalon van, ekkor mindket oldalhoz tartozik
					pos.push_back(pts[i]);
					neg.push_back(pts[i]);
				}
				sign1 = sign2; //a kovetkezo pont elojelet mar kiszamoltuk
			}
			return{ Polygon2<T>(std::move(neg)), Polygon2<T>(std::move(pos))};
		}

		//Konvex darabokra szeleteli az esetleg konkav polygont.
		//Jelenleg haromszogekre bontja, azonban ezt fejelszteni lehet. (TODO)
		//A polygon ful levagas modszeret alaklmazza.
		//TODO: ezt kivettem mert hibasan detektalja a fuleket, de kesobb kellhet
		//std::vector<Polygon2<T>> convex_partitions() const {
		//	bool cc = is_ccw();
		//	std::vector<Vector2<T>> tmp = pts;
		//	std::vector<Polygon2<T>> res;
		//	while (tmp.size() > 3){
		//		int i = 1;
		//		while (ccw(tmp[i-1], tmp[i], tmp[(i + 1) % tmp.size()]) != cc) ++i;
		//		res.push_back(Polygon2<T>(tmp.begin() + i - 1, tmp.begin() + i + 2));
		//		tmp.erase(tmp.begin() + i);
		//	}
		//	if (tmp.size() == 3) res.push_back(Polygon2<T>(std::move(tmp)));
		//	return res;
		//}

		//a Sutherland–Hodgman algoritmus alapjan megtalalja a masik polygon beleeso reszet
		//Polygon2<T> convex_clip(const Polygon2<T>& p) const {
		//	std::vector<Vector2<T>> outputlist = p.pts;
		//	int inside = ccw(pts[0], pts[1], pts[2]) ? 1 : -1;
		//	for (int i = 0; i < size(); ++i){
		//		Vector2<T> tmp = pts[(i + 1) % size()] - pts[i];
		//		Line<T> edge({-tmp.y,tmp.x},pts[i]);
		//		std::vector<Vector2<T>> inputlist = std::move(outputlist);
		//		outputlist.clear();
		//		Vector2<T> S = inputlist.back();
		//		for (const Vector2<T>& E : inputlist){
		//			T clfe = edge.classify_point(E);
		//			T clfs = edge.classify_point(S);
		//			if (clfe*inside >= 0){
		//				outputlist.push_back(E);
		//			}
		//			if (clfs*clfe < 0){
		//				T all = abs(clfe) + abs(clfs);
		//				outputlist.push_back((abs(clfs) / all)*E + (abs(clfe)/all)*S);;
		//			}
		//			S = E;
		//		}
		//	}
		//	return Polygon2<T>(std::move(outputlist));
		//}

		//mukodnie kell konvex es konkav esetre is
		bool contains(const Polygon2<T>& p) const {
			for (const Vector2<T>& pt : p) {
				if (!contains(pt))
					return false;
			}
			return true;
		}

		bool contains(const Vector2<T>& v) const {
			Line<T> line( Vector2<T>(0,1) , v);
			bool inside = false;
			int n = size();
			for (int i = 0; i < n; ++i) {
				T c1 = line.classify_point(points(i)),
				  c2 = line.classify_point(points((i + 1) % n));
				if (c1 * c2 < 0) {
					T all = abs(c1) + abs(c2);
					Vector2<T> pt = abs(c2) / all * points(i) + abs(c1) / all * points((i + 1) % n);
					if (pt.x > v.x) {
						inside = !inside;
					}
				}
				else if (c1 == 0 && points(i).x > v.x) {
					inside = !inside;
				}
			}
			return inside;
		}

		Polygon2<T> convex_clip(const Polygon2<T>& p) const {
			Polygon2<T>  output = p;
			bool cc = ccw(pts[0], pts[1], pts[2]);
			for (int i = 0; i < size() && output.size(); ++i) {
				Vector2<T> tmp = pts[(i + 1) % size()] - pts[i];
				Line<T> edge({ -tmp.y,tmp.x }, pts[i]);
				if (cc) {
					output = output.cut_by(edge).positive;
				}
				else {
					output = output.cut_by(edge).negative;
				}
			}
			return output;
		}

	};

}

#endif