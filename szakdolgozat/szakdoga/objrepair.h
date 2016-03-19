#ifndef OBJREPAIR_H_INCLUDED
#define OBJREPAIR_H_INCLUDED

//
// Keszitette: Toth Mate
// Ez a header fajl ellatja az egymashoz kozeli pontokbol allo sorozatok javitasat.
// Felhasznalhato faljok betoltesenel illetve barmely algoritmusban mely folyamatosan gyujti a pontokat.
//

#include <vector>
#include <algorithm>
#include <functional>
#include "vectors.h"
#include "indexiterator.h"

namespace approx{

	template <class T> class RepairVector{
		std::vector<Vector3<T>> vecs; //a konkret vektorok
		std::vector<int> ind_map; //egyes indexeket atiranyito lekepezo vektor

		std::function<T(const Vector3<T>&, const Vector3<T>&)> dist_fun; //tavolsag fv.
		T eps; //ekvivalencia sugar
		
		static T def_dist(const Vector3<T>& a, const Vector3<T>& b){
			return (a - b).length();
		}

	public:
		//epszilon sugaru euklideszi tavolsagban levo pontok egyenlonek tekintese
		RepairVector(T epsilon) : dist_fun(def_dist),eps(epsilon){}

		//a megadott tavolsag fuggvegy szerinti epszilon sugarban levo pontok egyenlonek tekintese
		RepairVector(T epsilon, const std::function<T(const Vector3<T>&, const Vector3<T>&)>& func) : dist_fun(func), eps(epsilon){}

		typedef ConstIndexIterator<T> ConstIterator;
		typedef IndexIterator<T> Iterator;


		//az adott indexrol eldonti valojaban hanyadik elemre mutat
		int transform_index(int ind) const {
			return ind_map[id];
		}

		//a megadott indexsorozatra elvegzi az indextranszformaciot
		std::vector<int> transform_range(const std::vector<int>& inds) const {
			std::vector<int> res;
			res.reserve(inds.size());
			for (int i : inds){
				res.push_back(ind_map[i]);
			}
			return res;
		}

		//elem lekerdezese
		Vector3<T> operator[](int ind) const {
			return vecs[ind_map[ind]];
		}

		//elemszam
		int size() const {
			return ind_map.size();
		}

		ConstIterator begin() const { return ConstIterator(&vecs,&ind_map,0); }
		ConstIterator end()   const { return ConstIterator(&vecs, &ind_map, size()); }
		Iterator begin() { return Iterator(&vecs, &ind_map, 0); }
		Iterator end()   { return Iterator(&vecs, &ind_map, size()); }

		//taroloba helyezes ellenorzessel
		void push_back(const Vector3<T>& v){
			int i = 0,n=size();
			while (i < n && eps < dist_fun(v, vecs[i])){ ++i; }
			ind_map.push_back(i);
			if (i == n) vecs.push_back(v);
		}

		//konvertalas vektorra az ismetlodesekkel egyutt
		operator std::vector<Vector3<T>>() const {
			return std::vector<Vector3<T>>(begin(), end());
		}

		//a valojaban kello egyedi vektorok listaja
		std::vector<Vector3<T>> needed_vecs() const {
			return vecs;
		}

	};

	//nulla tavolsaggal mukodik, viszont gyorsabban tud keresni mint a masik tipus
	//pontos ismetlodesek kiszuresere valo
	template <class T > class NullRepair {
		struct Less { //rendezesi muvelet a map tipussal valo hasznalathoz
			bool operator ()(const Vector3<T>& a, const Vector3<T>& b) const {
				return a.x < b.x ||
					(a.x == b.x && a.y < b.y) ||
					(a.x == b.x && a.y == b.y && a.z <b.z);
			}
		};

		std::vector<Vector3<T>> vecs;
		std::vector<int> inds;
		std::map<Vector3<T>, int, Less> vmap;

	public:
		typedef ConstIndexIterator<T> ConstIterator;
		typedef IndexIterator<T> Iterator;

		ConstIterator begin() const { return ConstIterator(&vecs, &inds, 0); }
		ConstIterator end()   const { return ConstIterator(&vecs, &inds, size()); }
		Iterator begin() { return Iterator(&vecs, &inds, 0); }
		Iterator end() { return Iterator(&vecs, &inds, size()); }

		void push_back(const Vector3<T>& v) {
			if (!vmap.count(v)) {
				vmap[v] = (int)vecs.size();
				vecs.push_back(v);
			}
			inds.push_back(vmap[v]);
		}
		int transform_index(int ind) const { return inds[ind]; }
		int size()const { return vecs.size(); }
		Vector3<T> operator[](int ind) const {
			return vecs[inds[ind]];
		}
		std::vector<int> transform_range(const std::vector<int>& _inds) const {
			std::vector<int> res;
			res.reserve(_inds.size());
			for (int i : _inds) {
				res.push_back(inds[i]);
			}
			return res;
		}
		std::vector<Vector3<T>> needed_vecs() const { return vecs; }
		
		//konvertalas vektorra az ismetlodesekkel egyutt
		operator std::vector<Vector3<T>>() const {
			return std::vector<Vector3<T>>(begin(), end());
		}

	};

}

#endif