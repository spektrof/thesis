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

#include "objrepair.h"
#include "body.h"

namespace approx{

	template <class T> class ObjectLoader;

	//Parametere tetszoleges Vector3 kompatibilis skalar
	template <class T> class TargetBody{
		std::vector<Vector3<T>> vecs,normals; //pont es normalvektor tarolok
		std::vector<Face<T>> faces; //lapok
		Body<T> bdy; //test
		T scale;
		Vector3<T> trans;
		static std::vector<int> range(size_t n){ //segedfuggveny az indexek eloallitasahoz
			std::vector<int> res;
			res.reserve(n);
			for (size_t i = 0; i < n; ++i){
				res.push_back((int)i);
			}
			return res;
		}

		void move_by(const Vector3<T>& v) {
			for (auto it = vecs.begin(); it != vecs.end(); ++it) {
				*it += v;
			}
		}

		void rescale(T scl) {
			for (auto it = vecs.begin(); it != vecs.end(); ++it) {
				*it *= scl;
			}
		}

	public:
		TargetBody() : bdy(nullptr, {}),scale(1){}
		//masolo es mozgato konstruktorok
		TargetBody(const TargetBody& t) : vecs(t.vecs), normals(t.normals), bdy(t.bdy.migrate_to(&faces)),scale(t.scale){
			faces.reserve(t.faces.size());
			for (const Face<T>& f : t.faces) { faces.push_back(f.migrate_to(&vecs, &normals)); }
		}
		//a koltoztetesi pointer atallitas miatt a mozgato konstruktor is linearis ideju
		TargetBody(TargetBody&& t) : vecs(std::move(t.vecs)), normals(std::move(t.normals)), bdy(std::move(t.bdy.migrate_to(&faces))),scale(t.scale){
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

		//TODO
		void ensure_safety() {
			NullRepair<T> rep,nrep;
			for (const Vector3<T>& v : vecs) {
				rep.push_back(v);
			}
			for (const Vector3<T>& v : normals) {
				nrep.push_back(v);
			}
			vecs = rep.needed_vecs();
			normals = nrep.needed_vecs();
			for (Face<T>& f : faces) {
				for (int& ind : f.indicies()) {
					ind = rep.transform_index(ind);
				}
				f.normal_index() = nrep.transform_index(f.normal_index());
			}
		}

		void transform_to_origo( T cube_size) {
			if (trans.length() > 0 || scale != 1) transform_back();
			trans = -(body().centroid());
			move_by(trans);
			Vector3<T> vmin = vecs[0], vmax = vecs[0];
			for (int i = 1; i < vecs.size();++i) {
				vmin.x = std::min(vmin.x, vecs[i].x);
				vmin.y = std::min(vmin.y, vecs[i].y);
				vmin.z = std::min(vmin.z, vecs[i].z);
				vmax.x = std::max(vmax.x, vecs[i].x);
				vmax.y = std::max(vmax.y, vecs[i].y);
				vmax.z = std::max(vmax.z, vecs[i].z);
			}
			Vector3<T> diff = vmax - vmin;
			T maxdist = std::max(diff.x, std::max(diff.y, diff.z));
			scale = cube_size / maxdist;
			rescale(scale);
		}

		void transform_back() {
			rescale(1 / scale);
			scale = 1;
			move_by(-trans);
			trans = { 0,0,0 };
		}

		T inverse_scale() const {
			return 1 / scale;
		}

		Vector3<T> inverse_transform() const {
			return -trans;
		}

		friend class ObjectLoader<T>;

	};

}

#endif