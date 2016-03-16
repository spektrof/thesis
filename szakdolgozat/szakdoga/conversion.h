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

namespace approx{
	typedef glm::vec2 Vec2;
	typedef glm::vec3 Vec3;
	typedef unsigned short Index;


	//minden rajzolasi infot tartalmazo adaszerkezet
	struct BodyList{
		std::vector<Vec3> points; //csucspontok
		std::vector<Index> indicies; //indexek amik a pontokra es normalokra mutatnak
		std::vector<Index> index_ranges; //az egymast koveto elemek az indexek szamai
	};

	template <class T> Vec3 convert(const Vector3<T>& p){
		return Vec3(p.x, p.y, p.z);
	}

	template <class T> Vector3<T> convert(const Vec3& v){
		return {v.x, v.y, v.z};
	}

	//egyetlen test adatainak kinyerese glm tipusokat hasznalo haromszogelt modellbe kirajzoltatashoz
	//CCW orientalt, OpenGL GL_TRIANGLES modban megjelenitve
	template <class T> BodyList drawinfo(const Body<T>& body){
		BodyList res;
		const std::vector<Vector3<T>>& vs = *body.faces(0).vertex_container();
		const std::vector<Vector3<T>>& ns = *body.faces(0).normal_container();
		res.points.reserve(vs.size());
		for (const Vector3<T>& p : vs){
			res.points.push_back(convert(p));
		}
		res.index_ranges.push_back(0);
		for (const Face<T>& f : body){
			if (f.size()) {
				Vector3<T> n = cross(f.points(2) - f.points(1), f.points(0) - f.points(1));
				int prev = dot(n, f.normal()) > 0 ? 1 : 0;
				for (int i = 2; i < f.size(); ++i) { //ccw-be sorolom es ugy veszem hogy kinn tudjak majd a normalist szamolni
					res.indicies.push_back(f.indicies(0));
					res.indicies.push_back(f.indicies(i - prev));
					res.indicies.push_back(f.indicies(i - 1 + prev));
				}
			}
		}
		res.index_ranges.push_back(res.indicies.size());
		return res;
	}

	//testek kozos adatai egy csomagban, haromszogelve
	template <class BodyIterator,class T> BodyList drawinfo(BodyIterator first, BodyIterator last){
		BodyList res;
		const Body<T>& b = *first;
		const std::vector<Vector3<T>>& vs = *b.faces(0).vertex_container();
		const std::vector<Vector3<T>>& ns = *b.faces(0).normal_container();
		res.points.reserve(vs.size());
		for (const Vector3<T>& p : vs){
			res.points.push_back(convert(p));
		}
		res.index_ranges.push_back(0);
		for (; first != last; ++first){
			res.index_ranges.push_back(res.index_ranges.back());
			for (const Face<T>& f : *first){
				if (f.size()) {
					Vector3<T> n = cross(f.points(2) - f.points(1), f.points(0) - f.points(1));
					int prev = dot(n, f.normal()) > 0 ? 1 : 0;
					for (int i = 2; i < f.size(); ++i) {
						res.indicies.push_back(f.indicies(0));
						res.indicies.push_back(f.indicies(i - prev));
						res.indicies.push_back(f.indicies(i - 1 + prev));
						res.index_ranges.back() += 3;
					}
				}
			}
		}
		return res;
	}

	template <class T> BodyList compact_drawinfo(const Body<T>& body){
		BodyList res;
		std::unordered_map<int, int> verts;
		for (const Face<T>& f : body){
			for (int i : f.indicies()){
				if (!verts.count(i)){
					verts[i] = res.points.size();
					res.points.push_back(convert(f.vertex_container()->operator[](i)));
				}
			}
			if (f.size()) {
				Vector3<T> n = cross(f.points(2) - f.points(1), f.points(0) - f.points(1));
				int prev = dot(n, f.normal()) > 0 ? 1 : 0;
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
}

#endif