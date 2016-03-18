#ifndef TARGETBODY_H_INCLUDED
#define TARGETBODY_H_INCLUDED

//
// Keszitette: Toth Mate
// Osztaly mely kezeli a kozelitendo testet minden adataval egyutt.
// A masolasa es a mozgatasa is linearis mert a lapok mutatoit felul kell irni
//
#include <utility>
#include <vector>
#include <string>

#include "body.h"

namespace approx{

	template <class T> class ObjectLoader;

	//Parametere tetszoleges Vector3 kompatibilis skalar
	template <class T> class TargetBody{
		std::vector<Vector3<T>> vecs,normals; //pont es normalvektor tarolok
		std::vector<Face<T>> faces; //lapok
		Body<T> bdy; //test

		static std::vector<int> range(size_t n){ //segedfuggveny az indexek eloallitasahoz
			std::vector<int> res;
			res.reserve(n);
			for (size_t i = 0; i < n; ++i){
				res.push_back((int)i);
			}
			return res;
		}

	public:
		TargetBody() : bdy(nullptr, {}){}
		//masolo es mozgato konstruktorok
		TargetBody(const TargetBody& t) : vecs(t.vecs), normals(t.normals), bdy(t.bdy.migrate_to(&faces)){
			faces.reserve(t.faces.size());
			for (const Face<T>& f : t.faces) { faces.push_back(f.migrate_to(&vecs, &normals)); }
		}
		//a koltoztetesi pointer atallitas miatt a mozgato konstruktor is linearis ideju
		TargetBody(TargetBody&& t) : vecs(std::move(t.vecs)), normals(std::move(t.normals)), bdy(std::move(t.bdy.migrate_to(&faces))){
			faces.reserve(t.faces.size());
			for (Face<T>& f : t.faces) { faces.push_back(f.migrate_to(&vecs, &normals)); }
		}

		//inicializalas pont, normalvektor es lap vektorokkal, a lapok kozul mindengyiket hasznalja
		TargetBody(const std::vector<Vector3<T>>& vec, const std::vector<Vector3<T>>& nrm, const std::vector<Face<T>>& fac)
			: vecs(vec), normals(nrm), bdy(&faces, std::move(range(fac.size()))) {
			faces.reserve(fac.size());
			for (const Face<T>& f : fac) { faces.push_back(f.migrate_to(&vecs, &normals)); }
		}
		TargetBody(std::vector<Vector3<T>>&& vec, std::vector<Vector3<T>>&& nrm, std::vector<Face<T>>&& fac)
			: vecs(vec), normals(nrm), bdy(&faces, std::move(range(fac.size()))){
			faces.reserve(fac.size());
			for (Face<T>& f : fac) { faces.push_back(f.migrate_to(&vecs, &normals)); }
		}

		//a konkret test kinyerese
		const Body<T>& body() const{
			return bdy;
		}

		//pontokat tartalmazo vektor
		const std::vector<Vector3<T>>& vertex_container() const { return vecs; }
		//normalisokat tartalmazo vektor
		const std::vector<Vector3<T>>& normal_container() const { return normals; }
		//lapokat tartalmazo vektor
		const std::vector<Face<T>>&    face_container()   const { return faces; }

		//masolo ertekadas
		TargetBody& operator = (const TargetBody& t) {
			vecs = t.vecs;
			normals = t.normals;
			faces.clear();
			faces.reserve(t.faces.size());
			for (const Face<T>& f : t.faces){
				faces.push_back(f.migrate_to(&vecs, &normals));
			}
			bdy = t.bdy.migrate_to(&faces);
			return *this;
		}


		//mozgato ertekadas, linearis idoben a lapokmutatok atalaitasa miatt
		TargetBody& operator = (TargetBody&& t){
			vecs = std::move(t.vecs);
			normals = std::move(t.normals);
			faces.clear();
			faces.reserve(t.faces.size());
			for (Face<T>&& f : t.faces){
				faces.push_back(f.migrate_to(&vecs, &normals));
			}
			bdy = std::move(t.bdy.migrate_to(&faces));
			return *this;
		}

		//friend bool load_obj(const std::string&,TargetBody<T>&);
		friend class ObjectLoader<T>;

	};

}

#endif