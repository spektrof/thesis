#ifndef CONVEXATOM_H_INCLUDED
#define CONVEXATOM_H_INCLUDED

//
// Keszitette: Toth Mate
// Konvex testeket reprezentalo adatszerkezet melyet az approximacio atomjaihoz hasznalunk.
// Kepes a sajat terfogatanak es fontosabb geometriai attributumainak kiszamolasara,
// valamint eretelmezve van rajta a sikkal vagas muvelete
//

#include <utility>
#include <algorithm>
#include <iterator>
#include <memory>
#include <map>
#include <set>
#include "body.h"
#include "planes.h"

namespace approx{

	//parameterezheto barmely Vector3-al kompatibilis skalar tipussal, melyre direkt,
	//vagy implicit konverzioval ertelmezve van az atan2 fuggveny
	template <class T> class ConvexAtom : public Body < T > {

		struct Less{ //rendezesi muvelet a map tipussal valo hasznalathoz
			bool operator ()(const Vector3<T>& a,const Vector3<T>& b) const{
				return a.x < b.x ||
					(a.x == b.x && a.y < b.y) ||
					(a.x == b.x && a.y == b.y && a.z <b.z);
			}
		};

		struct SurfacePoly{ //2 dimenzios lenyomatokkent tarolom a celtest metszetlapjait az atommal
			Plane<T> plane;
			std::vector<std::pair<Polygon2<T>,bool>> poly; //vetuletek, a logikai ertek pontosan akkor igaz ha nem kimetszo sokszog (pl fank belseje)
			SurfacePoly(const Plane<T>& pl) : plane(pl) {}
			//osszegzem az egyes polygonok teruletet
			T area() const { 
				T s = 0;
				for (const std::pair<Polygon2<T>,bool>& p : poly){
					s += p.first.area() * (p.second ? 1 : -1);
				}
				return s;
			}
		};

		typedef std::shared_ptr<SurfacePoly> PolyPtr;

		std::vector<std::shared_ptr<SurfacePoly>> f_poly;
		const Body<T>* target;

		static std::pair<int, int> decide_pair(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
			if (p1.first == p1.first)
				return std::pair<int, int>{p1.second, p2.second};
			if (p2.second == p1.second)
				return std::pair<int, int>(p1.first, p2.first);
			if (p1.first == p2.second)
				return std::pair<int, int>(p1.second, p2.first);
			else
				return std::pair<int, int>(p1.first, p2.second);
		}

	public:

		ConvexAtom(std::vector<Face<T>>* f, const std::vector<int>& i,const Body<T>* targ): Body<T>(f,i), target(targ){
			f_poly.reserve(size());
			for (int i = 0; i < size(); ++i) f_poly.push_back(std::make_shared<SurfacePoly>(faces(i).to_plane()));
		}
		ConvexAtom(std::vector<Face<T>>* f, const std::vector<int>& i, const Body<T>* targ, const std::vector<std::shared_ptr<SurfacePoly>>& plist) : Body<T>(f, i), f_poly(plist), target(targ){}
		ConvexAtom(std::vector<Face<T>>* f, std::vector<int>&& i, const Body<T>* targ, std::vector<std::shared_ptr<SurfacePoly>>&& plist) : Body<T>(f, i), f_poly(plist), target(targ){}
		ConvexAtom(const ConvexAtom&) = default;
		ConvexAtom(ConvexAtom&&) = default;

		ConvexAtom& operator = (const ConvexAtom& a){
			Body<T>::operator=(a);
			f_poly = a.f_poly;
			target = a.target;
			return *this;
		}

		ConvexAtom& operator = (ConvexAtom&& a){
			Body<T>::operator=(a);
			f_poly = std::move(a.f_poly);
			target = a.target;
			return *this;
		}

		//approximalt test
		const Body<T>* target_body() const { return target; }

		//megadott taroloba masolas
		ConvexAtom migrate_to(std::vector<Face<T>>* fcs) const {
			return ConvexAtom(fcs, inds, target, f_poly);
		}
		//megadott taroloba mozgatas
		ConvexAtom migrate_to(std::vector<Face<T>>* fcs) {
			return ConvexAtom(fcs, std::move(inds), target, std::move(f_poly));
		}

		//Vagasi eredmeny mely minden lehetseges informaciot tartalmaz amely az esetleges visszavonashoz es utomuveletekhez
		struct CutResult{
			std::shared_ptr<ConvexAtom<T>> positive, negative; //a ket keletkezett atom
			int points_added; //ennyi pontot adtunk hozza
			int faces_added; //ennyi lapot adtunk hozza

			int neg_cut_face, pos_cut_face; //a negativ illetve pozitiv oldalnak ezek lesznek a vagas menti lapjai
			
			struct FaceSplit {
				int ind_in_neg_atom; //a negativ oldali atomba eso fel lap atom beli indexe
				int ind_in_pos_atom; //a pozitiv oldali atomba eso fel lap atom beli indexe
				int neg_face_ind;    //a negativ oldali atomba eso fel lap tarolo beli indexe
				int pos_face_ind;    //a pozitiv oldali atomba eso fel lap tarolo beli indexe
				int pt_ind1, pt_ind2;//a vagasnal keletkezett uj csucspontok
			};
			
			std::map<int, FaceSplit> cut_map; //hozzarendeli az elvagott lapokhoz a keletkezett fel lapokat


		};

		// az atom elvagasa adott sikkal
		// a muveletigeny linearis a lapok szamaban valamint valodi vagasnal linearitmikus a vagasi pontok szamaban
		CutResult cut_by(const Plane<T>& p) const {
			std::vector<int> pt_ids, //a hozzaadott pontok indexei
							 pos_faces, //a pozitiv oldali lapok indexei
							 neg_faces; //a negativ oldali lapok indexei
			std::vector<std::shared_ptr<SurfacePoly>> pos_poly, neg_poly; //az egyes oldalakra kerulo vetuletek
			int faces_added=0, pts_added=0; //ennyi pont es lap kerult hozzaadasra a taroloban
			Vector3<T> avg_pt; //kozeppont ami korul rendezzuk az oldalakat
			int n_cut_f = -1, //negativ oldali vagas feloli lap
				p_cut_f = -1; //pozitiv oldali vagas feloli lap
			std::map<int, typename CutResult::FaceSplit> cut_map; //hozzarendeli az elvagott lapokhoz a keletkezett fel lapokat
			std::vector<Vector3<T>>& vc = *faces(0).vertex_container(); //rovid hozzaveres a vertexekhez
			std::map<Vector3<T>, int,Less> ptbuffer;
			std::pair<Vector3<T>, Vector3<T>> base = p.ortho2d();
			for (int i = 0; i < size();++i){ //vegegiteralok minden lapon es elvagom oket a sikkal, kezelve a hamis vagasokat
				Face<T>::CutResult cut = faces(i).cut_by(p,ptbuffer);
				if (cut.pt_inds.size() < cut.positive.size() && cut.pt_inds.size() < cut.negative.size()){ //valodi vagas tortent, mindket oldalon valid sokszog all
					pos_poly.push_back(std::make_shared<SurfacePoly>(f_poly[i]->plane));
					neg_poly.push_back(std::make_shared<SurfacePoly>(f_poly[i]->plane));
					pts_added += cut.points_added; //osszegzem a hozzaadott uj pontokat
					_faces->push_back(cut.negative); //a kapott lapokat szortirozom az uj sokszogekbe
					_faces->push_back(cut.positive); 
					faces_added += 2; //2 uj lap kerult a taroloba
					pos_faces.push_back(_faces->size() - 1);
					neg_faces.push_back(_faces->size() - 2);
					cut_map[indicies(i)] = {(int)neg_faces.size()-1,
											(int)pos_faces.size()-1,
											neg_faces.back(),
											pos_faces.back(),
											cut.pt_inds.front(),
											cut.pt_inds.back()};
					pt_ids.push_back(cut.pt_inds.front());
					pt_ids.push_back(cut.pt_inds.back());
					avg_pt += vc[pt_ids[pt_ids.size() - 1]];
					avg_pt += vc[pt_ids[pt_ids.size() - 2]];
					//gondoskodnom kell az elvagott vetuletek elosztasarol a ket lap kozott
					Line<T> line = f_poly[i]->plane.intersection_line(p);
					for (const std::pair<Polygon2<T>,bool>& poly : f_poly[i]->poly) { //vegigiteralom az ideeso vetuleteket
						if (poly.first.size() > 2) {
							typename Polygon2<T>::CutResult cut = poly.first.cut_by(line); //elvagom a vetuletet
																						//ha a vagas adott oldali eredmenye valodi sokszog akkor felveszem az adott oldalra
							if (cut.positive.size() >= 3) {
								pos_poly.back()->poly.emplace_back(cut.positive, poly.second);
							}
							if (cut.negative.size() >= 3) {
								neg_poly.back()->poly.emplace_back(cut.negative, poly.second);
							}

						}
					}
				}
				else{//a vagas nem krealt uj lapokat, de lehet hogy egy el vagy pont raesik
					if (cut.pt_inds.size()){//raeso pont vagy pontok
						pt_ids.push_back(cut.pt_inds.front());
						pt_ids.push_back(cut.pt_inds.back());
					}
					if (cut.pt_inds.size() < cut.positive.size()){ //a pozitiv oldalra kerul az egesz lap
						pos_faces.push_back(indicies(i));
						pos_poly.push_back(f_poly[i]);
					}
					else{ //a negativ oldalra kerul az egesz lap
						neg_faces.push_back(indicies(i));
						neg_poly.push_back(f_poly[i]);
					}
				}
			}

			if (pt_ids.size()){ //a vagasnak volt hatasa, keletkezett lap
				avg_pt /= static_cast<T>(pt_ids.size()); //kozeppont szamitasa
				// a kapott pontokat a sulypont korul a lap sikjan forgasszog szerint rendezem
				//a kapott lap konvex, tehat az ismetlodesek kiszurese utan megkapom a megfelelp sokszoget
				
				auto base = p.ortho2d();
				Vector3<T> vx = base.first, //x vektor a sikon
					vy = base.second; //y vektor a sikon
				//rendezes a tangensbol visszanyert szog alapjan
				std::sort(pt_ids.begin(), pt_ids.end(), [&](int a, int b){
					Vector3<T> v1 = vc[a] - avg_pt, v2 = vc[b] - avg_pt;
					T x1 = dot(v1, vx), y1 = dot(v1,vy),
						x2 = dot(v2, vx), y2 = dot(v2,vy);
					return atan2(x1, y1) < atan2(x2, y2);
				});

				//a rendezett pontokbol minden masodik egyedi bekerul a sokszogre
				std::vector<int> new_fc{pt_ids[0]};
				for (int i = 2; i < (int)pt_ids.size(); i += 2){
					if(vc[pt_ids[i]] != vc[new_fc.back()]) new_fc.push_back(pt_ids[i]); //egy csucsnal lehet hogy tobb el osszefut, nem akarunk egymas utan ugyanolyan pontokat
				}

				_faces->emplace_back(faces(0).vertex_container(),new_fc, faces(0).normal_container(),p.normal());
				Polygon2<T> clipper = _faces->back().to_2d(base.first,base.second); //a lap a sik koordinatarendszerebe athelyezve
				std::reverse(new_fc.begin(),new_fc.end());
				_faces->emplace_back(faces(0).vertex_container(), std::move(new_fc), faces(0).normal_container(),p.normal()*-1);
				faces_added+=2;
				n_cut_f = _faces->size() - 2;
				p_cut_f = _faces->size() - 1;
				neg_faces.push_back(n_cut_f);
				pos_faces.push_back(p_cut_f);
				//a vetuleteket is clippelni kell
				pos_poly.push_back(std::make_shared<SurfacePoly>(p));
				neg_poly.push_back(pos_poly.back());
				std::vector<std::pair<Polygon2<T>,bool>> surf = target->cut_surface(p);
				for (const std::pair<Polygon2<T>,bool>& e : surf){
					Polygon2<T> clipped = clipper.convex_clip(e.first);
					if (clipped.size() > 2) {
						pos_poly.back()->poly.emplace_back(clipped,e.second);
					}
				}
			}
			
			return{ std::make_shared<ConvexAtom<T>>(_faces,std::move(pos_faces),target,std::move(pos_poly)),
					std::make_shared<ConvexAtom<T>>(_faces,std::move(neg_faces),target,std::move(neg_poly)),
					pts_added,
					faces_added,
					n_cut_f,
					p_cut_f,
					std::move(cut_map)};
		}

		//megvizsgalja, hogy a pont bele esik-e
		bool point_inside(const Vector3<T>& pt) const {
			for (const Face<T>& f : *this){
				if (f.to_plane().classify_point(pt) > 0) return false;
			}
			return true;
		}

		//megszamolja, hogy a cel testnek hany pontja esik az atomba
		int target_vertex_count_inside() const{
			int cnt = 0;
			std::set<Vector3<T>> pts;
			for (const Face<T>& face : *target){
				for (const Vector3<T>& p : face){
					if (point_inside(p)) pts.insert(p);
				}
			}
			return pts.size();
		}

		//metszetterfogat szamitas a celtesttel
		T intersection_volume() const {
			T sum = 0;
			for (const Face<T>& f : *target){
				std::vector<Vector3<T>> tmp_vert, tmp_norm;
				ConstFaceIterator it = begin();
				Face<T> clipf = f;
				//amennyiben a lap pontosan a sikon van, a vetuletet kell beszamitanom, magat a lapot nem, ezzel elkerulve a az ismetlodest
				bool onplane = f.to_plane().example_point() == it->to_plane().example_point();
				while (it != end() && clipf.size() >= 3 && !onplane){
					typename Face<T>::CutResult cut = clipf.cut_by(it->to_plane(),&tmp_vert,&tmp_norm);
					onplane = cut.points_added == 0 && cut.pt_inds.size() == clipf.size();
					clipf = cut.negative;
					++it;
				}
				if (clipf.size() >= 3 && !onplane){

					//T x = clipf.to_2d().area();
					//T y = clipf.to_plane().signed_distance();
					//Vector3<T> norm1 = clipf.to_plane().normal(),
					//	       norm2 = clipf.normal();

					sum += clipf.to_2d().area()*clipf.to_plane().signed_distance();
				}
			}

			for (int i = 0; i < size(); ++i){
				sum += f_poly[i]->area()*faces(i).to_plane().signed_distance();
			}
			return sum / static_cast<T>(3);
		}

		//Fourier-egyutthato, azaz a metszet es a teljes terfogatanak a hanyadosa
		T fourier() const {
			return intersection_volume() / volume();
		}

		//az i-edik metszet-lenyomat 
		PolyPtr surf_imprints(int i) const {
			return f_poly[i];
		}

		//a taroloban adott indexel rendelkezo lapot ebben az atomban lecserelem a taroloban adott indexszel rendelkezo lapokra
		void replace_face_with(int real_ind, int rep_ind1, int rep_ind2, const PolyPtr& p1, const PolyPtr& p2,int cutpt1,int cutpt2) {
			//megkeresem melyik lapot kell elfeleznem a masik kettovel
			int ind = std::find(indicies().begin(), indicies().end(), real_ind) - indicies().begin();
			indicies()[ind] = rep_ind1;
			f_poly[ind] = p1;
			indicies().push_back(rep_ind2);
			f_poly.push_back(p2);
			//majd a tobbi lapon vegrehajtom a kello modositasokat hogy a szomszedsagi viszonyok ne seruljenek
			std::pair<int, int> neigh11 = (*_faces)[rep_ind1].neighbours_of(cutpt1),
								neigh21 = (*_faces)[rep_ind2].neighbours_of(cutpt1),
								neigh12 = (*_faces)[rep_ind1].neighbours_of(cutpt2),
								neigh22 = (*_faces)[rep_ind2].neighbours_of(cutpt2);
			//meghatarozom melyik pontok koze kell beszurni
			std::pair<int, int> pair1 = decide_pair(neigh11,neigh21);
			std::pair<int, int> pair2 = decide_pair(neigh12, neigh22);
			bool in1 = false, in2 = false;
			int n=size()-1;
			for (int i = 0; i < n && !(in1 && in2); ++i) {
				if (i != ind) {
					if (!in1) {
						in1 = faces(i).insert_index(pair1.first, pair1.second, cutpt1);
						if(!in1 && !in2) faces(i).insert_index(pair2.first, pair2.second, cutpt2);
					}
					else if (!in2) in2 = faces(i).insert_index(pair2.first, pair2.second, cutpt2);
				}
			}
 		}

		//aleiro lapok sulypontjainak atlaga, nem egyenlo a sulyponttal
		Vector3<T> avg_point() const {
			Vector3<T> c;
			for (const Face<T>& f : *this) {
				c += f.centroid();
			}
			return c / static_cast<T>(size());
		}

		int bad_normal_ind() const {
			Vector3<T> cent = avg_point();//centroid();
			int i = 0;
			for (const Face<T>& f : *this) {
				if (dot(f.normal(), cent - f.points(0)) > 0)
					return i;
				++i;
			}
			return -1;
		}

		//a celtest atomba eso lapindexeinek listaja
		std::vector<int> face_indicies_inside() const {
			std::vector<int> res;
			for (int ind : target->indicies()) {
				std::vector<Vector3<T>> tmp_vert, tmp_norm;
				ConstFaceIterator it = begin();
				Face<T> clipf = target->faces(ind);
				while (it != end() && clipf.size() >= 3) {
					typename Face<T>::CutResult cut = clipf.cut_by(it->to_plane(), &tmp_vert, &tmp_norm);
					clipf = cut.negative;
					++it;
				}
				if (clipf.size() >= 3) {
					res.push_back(ind);
				}
			}
			return res;
		}

		//az atomba eso lapok listaja
		std::vector<Face<T>> faces_inside() const {
			std::vector<Face<T>> res;
			for (const Face<T>& f : *target) {
				std::vector<Vector3<T>> tmp_vert, tmp_norm;
				ConstFaceIterator it = begin();
				Face<T> clipf = f;
				while (it != end() && clipf.size() >= 3) {
					typename Face<T>::CutResult cut = clipf.cut_by(it->to_plane(), &tmp_vert, &tmp_norm);
					clipf = cut.negative;
					++it;
				}
				if (clipf.size() >= 3) {
					res.push_back(f);
				}
			}
			return res;
		}

	};

}

#endif