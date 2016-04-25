#ifndef CONVERSION_H_INCLUDED
#define CONVERSION_H_INCLUDED

//
// Keszitette: Toth Mate
// Konverzios es kulso hasznalatra alkalmassa tevo eljarasok, tipusok.
// A belso adatszerkezeteket a glm csomag tipusaiba kepzem le.
//


#include <vector>
#include <unordered_map>
#include "glm/glm.hpp"
#include "body.h"
#include "convexatom.h"
#include "vectors.h"
namespace approx {
	typedef glm::vec2 Vec2;
	typedef glm::vec3 Vec3;
	typedef unsigned int Index;


	//minden rajzolasi informaciot tartalmazo adaszerkezet
	struct BodyList {
		std::vector<Vec3> points; //csucspontok
		std::vector<Index> indicies; //indexek amik a pontokra es normalokra mutatnak
		std::vector<Index> index_ranges; //az egymast koveto elemek az indexek szamai
	};

	//egyetlen lapnak es a raeso vetuleteknek vonalas megjelenitesehez
	//a polygonok kozul az elso mindig a lap
	struct PolyFace2D {
		std::vector<Vec2> points; //ezeket a pontokat hasznaljuk fel
		std::vector<Index> ranges; //az i. polygon [ranges[i];ranges[i+1]) intervallumon van eltarolva a pontok kozott
		std::vector<bool> outer; //pontosan akkor igaz ha az i. polygon kulso oldali
		Vec3 x_axis, y_axis; //a teljesseg miatt a sik ket tengelye a globalis vektorterben
		PolyFace2D() {}
		PolyFace2D(const PolyFace2D&) = default;
		PolyFace2D(PolyFace2D&&) = default;
		PolyFace2D& operator = (const PolyFace2D&) = default;
		PolyFace2D& operator = (PolyFace2D&&) = default;
	};

	//konverzio vektorbol glm vektorba
	template <class T> Vec3 convert(const Vector3<T>& p) {
		return Vec3((float)p.x, (float)p.y, (float)p.z);
	}

	//konverzio glm vektorbol vektorba
	template <class T> Vector3<T> convert(const Vec3& v) {
		return{ v.x, v.y, v.z };
	}

	//konverzio vektorbol glm vektorba
	template <class T> Vec2 convert(const Vector2<T>& v) {
		return Vec2((float)v.x, (float)v.y);
	}

	//konverzio glm vektorbol vektorb
	template <class T> Vector2<T> convert(const Vec2& v) {
		return{ v.x, v.y };
	}

	//egyetlen test adatainak kinyerese glm tipusokat hasznalo haromszogelt modellbe kirajzoltatashoz
	//CCW orientalt, OpenGL GL_TRIANGLES modban megjelenitve
	template <class T> BodyList drawinfo(const Body<T>& body) {
		BodyList res;
		const std::vector<Vector3<T>>& vs = *body.faces(0).vertex_container();
		res.points.reserve(vs.size());
		for (const Vector3<T>& p : vs) {
			res.points.push_back(convert(p));
		}
		res.index_ranges.push_back(0);
		for (const Face<T>& f : body) {
			if (f.size()) {
				int prev = f.is_ccw() ? 1 : 0;
				for (int i = 2; i < (int)f.size(); ++i) { //ccw-be sorolom es ugy veszem hogy kinn tudjak majd a normalist szamolni
					res.indicies.push_back((Index)f.indicies(0));
					res.indicies.push_back((Index)f.indicies(i - prev));
					res.indicies.push_back((Index)f.indicies(i - 1 + prev));
				}
			}
		}
		res.index_ranges.push_back((unsigned int)res.indicies.size());
		return res;
	}


	//testek kozos adatai egy csomagban, haromszogelve
	template <class BodyIterator, class T> BodyList drawinfo(BodyIterator first, BodyIterator last) {
		BodyList res;
		const Body<T>& b = *first;
		const std::vector<Vector3<T>>& vs = *b.faces(0).vertex_container();
		res.points.reserve(vs.size());
		for (const Vector3<T>& p : vs) {
			res.points.push_back(convert(p));
		}
		res.index_ranges.push_back(0);
		for (; first != last; ++first) {
			res.index_ranges.push_back(res.index_ranges.back());
			for (const Face<T>& f : *first) {
				if (f.size()) {
					int prev = f.is_ccw() ? 1 : 0;
					for (int i = 2; i < (int)f.size(); ++i) {
						res.indicies.push_back((Index)f.indicies(0));
						res.indicies.push_back((Index)f.indicies(i - prev));
						res.indicies.push_back((Index)f.indicies(i - 1 + prev));
						res.index_ranges.back() += 3;
					}
				}
			}
		}
		return res;
	}

	//kompakt info kinyerese, csak azoka a pontok kerulnek be amiket tenyleg felhasznalunk
	template <class T> BodyList compact_drawinfo(const Body<T>& body) {
		BodyList res;
		std::unordered_map<int, int> verts;
		for (const Face<T>& f : body) {
			for (int i : f.indicies()) {
				if (!verts.count(i)) {
					verts[i] = res.points.size();
					res.points.push_back(convert(f.vertex_container()->operator[](i)));
				}
			}
			if (f.size()) {
				int prev = f.is_ccw() ? 1 : 0;
				for (int i = 2; i < f.indicies().size(); ++i) {
					res.indicies.push_back(verts[f.indicies(0)]);
					res.indicies.push_back(verts[f.indicies(i - prev)]);
					res.indicies.push_back(verts[f.indicies(i - 1 + prev)]);
				}
			}
		}
		res.index_ranges = { 0, (Index)res.indicies.size() };
		return res;
	}

	//a lapokra eso metszetrajzolatok kinyerese
	template <class T> std::vector<PolyFace2D> drawinfo2d(const ConvexAtom<T>& a) {
		std::vector<PolyFace2D> result;
		for (int i = 0; i < a.size(); ++i) {
			PolyFace2D fac;
			fac.ranges.push_back(0);
			for (const Vector2<T>& p : a.faces(i).to_2d()) {
				fac.points.push_back(convert(p));
			}
			fac.ranges.push_back((Index)fac.points.size());
			fac.outer.push_back(true);
			auto base = a.surf_imprints(i)->plane.ortho2d();
			fac.x_axis = convert(base.first);
			fac.y_axis = convert(base.second);
			float dir = dot(a.surf_imprints(i)->plane.normal(), a.faces(i).normal()) < 0 ? -1.0f : 1.0f;
			for (const std::pair<Polygon2<T>, bool>& poly : a.surf_imprints(i)->poly) {
				if ((dir > 0 && poly.first.is_ccw()) || (dir < 0 && !poly.first.is_ccw())) {
					for (const Vector2<T>& p : poly.first) {
						fac.points.push_back(Vec2(dir * (float)p.x, (float)p.y));
					}
				}
				else {
					for (auto it = poly.first.points().rbegin(); it != poly.first.points().rend(); ++it) {
						fac.points.push_back(Vec2(dir * (float)it->x, (float)it->y));
					}
				}
				fac.outer.push_back(poly.second);
				fac.ranges.push_back((Index)fac.points.size());
			}
			result.push_back(fac);
		}

		return result;

	}

}

#endif