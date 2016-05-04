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
	// valamint elvegezheto rajta az "x > 0" es az std::abs abszolutertek muvelet.
	template<class T> class Polygon2{
		std::vector<Vector2<T>> pts; //a pontok felsorolasa
	public:
		typedef typename std::vector<Vector2<T>>::const_iterator ConstIterator; //iterator tipus konstans elereshez
		typedef typename std::vector<Vector2<T>>::iterator Iterator; //iterator tipus mely megengedi a 

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
		
		//elojeles terulet, elojele segithet annak eldonteseben hogy cw vagy ccw felsorolasban van megadva
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
			return std::abs(signed_area());
		}

		//pontosan akkor igaz, ha a pontos oramutatoval ellenkezo iranyban vannak felsorolva
		//a pontok szamaban linearis idoigenyu, de mukodik konkav esetre is
		bool is_ccw() const {
			return signed_area() < 0;
		}

		//vagasi eredmeny tipus
		struct CutResult{
			Polygon2 negative, //a vago egyenes negativ oldalara eso resz
					 positive; //a vago egyenes pozitiv oldalara eso resz
		};

		Polygon2<T> to_convex() const {
			if (size() < 3) return *this;
			std::vector<Vector2<T>> out{pts[0],pts[1]};
			bool cc = is_ccw();
			for (int i = 2; i < size(); ++i) {
				int j = out.size() - 1;
				while (j > 0 && ccw(out[j - 1], out[j], pts[i]) != cc) {
					--j;
					out.pop_back();
				}
				out.push_back(pts[i]);
			}
			while (out.size() > 3 && ccw(out[out.size() - 2], out[out.size() - 1], out[0]) != cc) out.pop_back();
			return Polygon2<T>(std::move(out));
		}
		//a megadott vonallal elvagva kapott sokszogek szamitasa
		//feltetelezzuk, hogy a sokszog konvex, ebbol kovetkezoen az eredmeny is az
		//muveletigeny linearis a csucspontok szamaban
		CutResult cut_by(const Line<T> l) const {
			if (!l.valid()) return{ *this,*this };
			std::vector<Vector2<T>> pos, neg;
			T sign1 = l.classify_point(pts[0]); //az aktualisan vizsgalt pont elhelyezkedese a sikhoz kepest
			const int n = size();
			for (int i = 0; i < n;++i){ //vegigiteralok a pontok kozott
				T sign2 = l.classify_point(pts[(i + 1) % n]); //a kovetkezo pont betajolasa
				if (sign1 < 0){//a pont a negativ oldalra esik
					neg.push_back(pts[i]);
					if (sign2 > 0){//a kovetkezo pont pozitiv, a ketto kozott vagas keletkezik
						T div = std::abs(sign1 / (std::abs(sign1)+std::abs(sign2)));
						Vector2<T> cpt = (1 - div)*pts[i] + div*pts[(i + 1) % n];
						pos.push_back(cpt);
						neg.push_back(cpt);
					}
				}
				else if (sign1 > 0){//a pont a pozitiv oldalra esik
					pos.push_back(pts[i]);
					if (sign2 < 0){//a kovetkezo pont negativ, a ketto kozott vagas keletkezik
						T div = std::abs(sign1 / (std::abs(sign1) + std::abs(sign2)));
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

		//pontosan akkor igaz, ha a masik sokszog beleesik a sokszogbe
		//mukodik konvex es konkav esetben is
		bool contains(const Polygon2<T>& p) const {
			for (const Vector2<T>& pt : p) {
				if (!contains(pt))
					return false;
			}
			return true;
		}

		//pontosan akkor igaz, ha a pont beleesik a sokszogbe
		bool contains(const Vector2<T>& v) const {
			Line<T> line( Vector2<T>(0,1) , v);
			bool inside = false;
			int n = size();
			for (int i = 0; i < n; ++i) {
				T c1 = line.classify_point(points(i)),
				  c2 = line.classify_point(points((i + 1) % n));
				if (c1 * c2 < 0) {
					T all = std::abs(c1) + std::abs(c2);
					Vector2<T> pt = std::abs(c2) / all * points(i) + std::abs(c1) / all * points((i + 1) % n);
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

		//A Sutherland-Hodgman algoritmus alapjan konvex kivagja a p polygon beeso reszet.
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

		//sulypont kiszamitasa
		Vector2<T> centroid() const {
			T sa = (-6 * signed_area());
			T cx = 0,cy=0;
			int n = size();
			for (int i = 0; i < n; ++i) {
				Vector2<T> pi = points(i),
						   pi1=points((i+1)%n);
				T mult = pi.x * pi1.y - pi1.x*pi.y;
				cx += (pi.x + pi1.x) * mult;
				cy += (pi.y + pi1.y)* mult;
			}
			return{ cx/sa,cy/sa };
		}

	};

}

#endif