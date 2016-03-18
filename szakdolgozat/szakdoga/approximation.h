#ifndef APPROXIMATION_H_INCLUDED
#define APPROXIMATION_H_INCLUDED

//
// Keszitette: Toth Mate
// Az approximacios folyamatot kezelo adatszerkezetet leiro tipus.
// A felhasznaloval valo interakcio ezen a szinten lep ervenybe
//

#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include "vectors.h"
#include "face.h"
#include "body.h"
#include "convexatom.h"
#include "targetbody.h"

namespace approx{
	
	//az esetlegesen keletkezo belso oldalak kezelesenek modja
	enum class InsideHandling {
		LeaveOut, //a belso oldalakat kihagyjuk 
		FlipInside, //a belso oldalak normalisait megforgatjuk
		AddInside, //a belso oldalak normalisait beken hagyjuk
	};


	//approximacios tarolo mely az osszes atomot tarolja
	//parameterei:
	// T: az approximacio skalar tipusa, kompatibilisnek kell lennie a Vector3 sablonnal, valamint az atan2 fuggvennyel mukodnie kell
	// AtomType: az atomtipus melyet az approximacio hasznal, publikus interfeszenek meg kell egyeznie a ConvexAtom<T> tipussal,
	//			 az approximacio soran ez a tipus lesz minden atom tipusa
	template <class T> class Approximation {
		typedef ConvexAtom<T> AtomType;
		const TargetBody<T>* target; //a cel test
		std::vector<Vector3<T>> vertices, normals; //pontok es normalisok
		std::vector<Face<T>> faces; //lapok
		std::vector<AtomType> _atoms; //atomok
		
		int last_cut; //az utolso vagasi muvelet atomjanak indexe
		typename AtomType::CutResult cut_res; //az utolso vagas eredmenye

		struct Connection {
			int other_atom; //-1 = kifele mutat -2 = befele mutat, -3 torolt, 0 vagy nagyobb normal atom
			int other_face;
			Connection(int atom=-1,int face=-1) : other_atom(atom),other_face(face){}

			Connection& operator = (const Connection&) = default;
		};
		
		std::vector<Connection> connections;

		//a kezdoatom egy tengelyek menten felhuzott kocka mely minden iranyban megadott hatarral a test korul helyezkedik el
		//kiszamitasanak ideje linearis a test pontjainak szamaban
		void starting_atom(T border){
			Vector3<T> vmin = target->body().faces(0).points(0),
				       vmax = target->body().faces(0).points(0);
			for (const Face<T>& f : target->body())
				for (const Vector3<T>& v : f){
					vmin.x = std::min(vmin.x, v.x);
					vmin.y = std::min(vmin.y, v.y);
					vmin.z = std::min(vmin.z, v.z);
					vmax.x = std::max(vmax.x, v.x);
					vmax.y = std::max(vmax.y, v.y);
					vmax.z = std::max(vmax.z, v.z);
				}
			vertices.push_back({ vmin.x - border, vmin.y - border, vmin.z - border });
			vertices.push_back({ vmax.x + border, vmin.y - border, vmin.z - border });
			vertices.push_back({ vmax.x + border, vmax.y + border, vmin.z - border });
			vertices.push_back({ vmin.x - border, vmax.y + border, vmin.z - border });
			vertices.push_back({ vmin.x - border, vmin.y - border, vmax.z + border });
			vertices.push_back({ vmax.x + border, vmin.y - border, vmax.z + border });
			vertices.push_back({ vmax.x + border, vmax.y + border, vmax.z + border });
			vertices.push_back({ vmin.x - border, vmax.y + border, vmax.z + border });
			normals.push_back({ 0,  0,-1});
			normals.push_back({ 1,  0, 0 });
			normals.push_back({ 0,  0, 1 });
			normals.push_back({ -1, 0, 0 });
			normals.push_back({ 0,  1, 0 });
			normals.push_back({ 0, -1, 0 });
			faces.emplace_back(&vertices, std::vector<int>{0, 1, 2, 3}, &normals, 0);
			faces.emplace_back(&vertices, std::vector<int>{1, 5, 6, 2}, &normals, 1);
			faces.emplace_back(&vertices, std::vector<int>{5, 4, 7, 6}, &normals, 2);
			faces.emplace_back(&vertices, std::vector<int>{4, 0, 3, 7}, &normals, 3);
			faces.emplace_back(&vertices, std::vector<int>{3, 2, 6, 7}, &normals, 4);
			faces.emplace_back(&vertices, std::vector<int>{0, 4, 5, 1}, &normals, 5);
			_atoms.emplace_back(&faces, std::vector<int>{0, 1, 2, 3, 4, 5},&target->body());
			connections.resize(6);
		}

		//eldonti hogy a kulso vagy belso extremalist kell hozzarendelni a szomszedokhoz
		Connection decide_extreme(const std::vector<int>& inds) {
			for (int x : inds) {
				if (connections[x].other_atom == -1) return { -1,-1 };
			}
			return { -2,-1 };
		}

		void undo() {
			cut_res.positive.reset();
			cut_res.negative.reset();
			faces.erase(faces.end() - cut_res.faces_added, faces.end());
			vertices.erase(vertices.end() - cut_res.points_added, vertices.end());
			normals.pop_back();
			normals.pop_back();
			last_cut = -1;
		}

		void choose_both() {
			_atoms[last_cut] = std::move(*static_cast<AtomType*>(cut_res.negative.get()));
			_atoms.push_back(*static_cast<AtomType*>(cut_res.positive.get()));

			connections.resize(faces.size());
			int posind = (int)_atoms.size() - 1;
			//a vagas menten a ket fel atom parban lesz
			connections[cut_res.neg_cut_face] = Connection( posind,cut_res.pos_cut_face );
			connections[cut_res.pos_cut_face] = Connection( last_cut,cut_res.neg_cut_face );
			for (const auto& e : cut_res.cut_map) { //bejarom a vago mapot
				if (connections[e.first].other_atom > -1) { //csatlakozik valamivel a regi lap
					//elvagom a masik lapot
					const Face<T>& otherf = faces[connections[e.first].other_face];
					faces.push_back(Face<T>(&vertices,
											faces[e.second.neg_face_ind].indicies().rbegin(),
											faces[e.second.neg_face_ind].indicies().rend(),
											&normals,
											otherf.normal_index()));
					connections.push_back(Connection(last_cut,e.second.neg_face_ind));
					faces.push_back(Face<T>(&vertices,
											faces[e.second.pos_face_ind].indicies().rbegin(),
											faces[e.second.pos_face_ind].indicies().rend(),
											&normals,
											otherf.normal_index()));
					connections.push_back(Connection(posind, e.second.pos_face_ind));
					//rendezem a kapcoslatokat
					connections[e.second.neg_face_ind] = Connection( connections[e.first].other_atom, (int)faces.size()-2 );
					connections[e.second.pos_face_ind] = Connection( connections[e.first].other_atom, (int)faces.size() - 1 );
					//lecserelem a masik atomban a lapot
					_atoms[connections[e.first].other_atom].replace_face_with(
						connections[e.first].other_face,
						(int)faces.size()-2,
						(int)faces.size()-1,
						_atoms[last_cut].surf_imprints(e.second.ind_in_neg_atom),
						_atoms.back().surf_imprints(e.second.ind_in_pos_atom)
						);
				}
			}
			//ha nem vagtam el egy lapot akkor be kell allitanom az uj kapcsolati adatokat
			//a negativ atom lapjaival semmi dolgom nincs, azok maradtak helyben,
			//de a pozitiv lapjainak a szembe szomszedjat be kell allitanom az uj indexre
			for (int ind : _atoms.back().indicies()) {
				if (connections[ind].other_face>=0)
					connections[connections[ind].other_face].other_atom = posind;
			}
			cut_res.positive.reset();
			cut_res.negative.reset();
			last_cut = -1;
		}

		void choose_negative() {
			_atoms[last_cut] = std::move(*static_cast<AtomType*>(cut_res.negative.get()));

			connections.resize(faces.size());
			//a vagas menten uresseg keletkezik
			Connection extreme=decide_extreme(cut_res.positive->indicies());
			connections[cut_res.neg_cut_face] = extreme;
			connections[cut_res.pos_cut_face] = Connection(-3,-1);
			for (const auto& e : cut_res.cut_map) { //bejarom a vago mapot
				if (connections[e.first].other_atom > -1) { //csatlakozik valamivel a regi lap
															//elvagom a masik lapot
					const Face<T>& otherf = faces[connections[e.first].other_face];
					faces.push_back(Face<T>(&vertices,
						faces[e.second.neg_face_ind].indicies().rbegin(),
						faces[e.second.neg_face_ind].indicies().rend(),
						&normals,
						otherf.normal_index()));
					connections.push_back(Connection( last_cut,e.second.neg_face_ind ));
					faces.push_back(Face<T>(&vertices,
						faces[e.second.pos_face_ind].indicies().rbegin(),
						faces[e.second.pos_face_ind].indicies().rend(),
						&normals,
						otherf.normal_index()));
					connections.push_back(extreme); // a pozitiv fel extremalis uresseget kap
					//rendezem a kapcoslatokat
					connections[e.second.neg_face_ind] = Connection( connections[e.first].other_atom, (int)faces.size() - 2 );
					connections[e.second.pos_face_ind] = Connection(-3, -1);
					//lecserelem a masik atomban a lapot
					_atoms[connections[e.first].other_atom].replace_face_with(
						connections[e.first].other_face,
						(int)faces.size() - 2,
						(int)faces.size() - 1,
						_atoms[last_cut].surf_imprints(e.second.ind_in_neg_atom),
						cut_res.positive->surf_imprints(e.second.ind_in_pos_atom)
						);
				}
			}
			for (int ind : cut_res.positive->indicies()) { //mivel kitoroltem a pozitivat ezert ott is uressegre fognak mutatni
				if (connections[ind].other_face>=0)
					connections[connections[ind].other_face] = extreme.other_atom;
			}


			cut_res.positive.reset();
			cut_res.negative.reset();
			last_cut = -1;
		}

		void choose_positive() {
			_atoms[last_cut] = std::move(*static_cast<AtomType*>(cut_res.positive.get()));
			
			connections.resize(faces.size());
			//a vagas menten uresseg keletkezik
			Connection extreme = decide_extreme(cut_res.negative->indicies());
			connections[cut_res.pos_cut_face] = extreme;
			connections[cut_res.neg_cut_face] = Connection(-3,-1);
			for (const auto& e : cut_res.cut_map) { //bejarom a vago mapot
				if (connections[e.first].other_atom > -1) { //csatlakozik valamivel a regi lap
															//elvagom a masik lapot
					const Face<T>& otherf = faces[connections[e.first].other_face];
					faces.push_back(Face<T>(&vertices,
						faces[e.second.neg_face_ind].indicies().rbegin(),
						faces[e.second.neg_face_ind].indicies().rend(),
						&normals,
						otherf.normal_index()));
					connections.push_back(extreme);
					faces.push_back(Face<T>(&vertices,
						faces[e.second.pos_face_ind].indicies().rbegin(),
						faces[e.second.pos_face_ind].indicies().rend(),
						&normals,
						otherf.normal_index()));
					connections.push_back(Connection( last_cut,e.second.pos_face_ind )); // a pozitiv fel extremalis uresseget kap
													//rendezem a kapcoslatokat
					connections[e.second.neg_face_ind] = Connection( -3, -1 );
					connections[e.second.pos_face_ind] = Connection( connections[e.first].other_atom, (int)faces.size() - 1 );
					//lecserelem a masik atomban a lapot
					_atoms[connections[e.first].other_atom].replace_face_with(
						connections[e.first].other_face,
						(int)faces.size() - 2,
						(int)faces.size() - 1,
						_atoms[last_cut].surf_imprints(e.second.ind_in_neg_atom),
						cut_res.negative->surf_imprints(e.second.ind_in_pos_atom)
						);
				}
			}
			for (int ind : cut_res.negative->indicies()) { //mivel kitoroltem a pozitivat ezert ott is uressegre fognak mutatni
				if (connections[ind].other_face>=0)
					connections[connections[ind].other_face] = extreme.other_atom;
			}


			cut_res.positive.reset();
			cut_res.negative.reset();
			last_cut = -1;
		}


	public:
		typedef T ScalarType;
		//iterator tipusok a konnyu bejaras erdekeben
		typedef typename std::vector<AtomType>::iterator Iterator;
		typedef typename std::vector<AtomType>::const_iterator ConstIterator;

		//konstruktor mely az approximalando testre mutato pointert es a kezdo kocka lapjainak kozelseget varja
		//futasideje linearis a celtest pontjainak szamaban
		Approximation(const TargetBody<T>* _target, T _border) : target(_target), last_cut(-1){
			starting_atom(_border);
		}

		Approximation(const Approximation& a) : target(app.target), vertices(app.vertices), normals(app.normals){
			faces.clear();
			_atoms.clear();
			for (const Face<T>& f : app.faces) {
				faces.push_back(f.migrate_to(&vertices, &normals));
			}
			for (const AtomType<T>& a : app._atoms) {
				_atoms.push_back(a.migrate_to(&faces));
			}
		}

		//masolo ertekadas, gondoskodik a koltoztetesnel fellepo pointer valtasrol
		Approximation& operator = (const Approximation& app){
			target = app.target;
			vertices = app.vertices;
			normals = app.normals;
			faces.clear();
			_atoms.clear();
			for (const Face<T>& f : app.faces){
				faces.push_back(f.migrate_to(&vertices, &normals));
			}
			for (const AtomType<T>& a : app._atoms){
				_atoms.push_back(a.migrate_to(&faces));
			}
			return *this;
		}

		//mozgato ertekadas, gondoskodnia kell a koltoztetesnel fellepo pointer atallitasrol,
		//ezert bar gyorsabb a masolasnal, megis linearis muveletigenye van
		Approximation& operator = (Approximation&& app){
			target = app.target;
			vertices = std::move(app.vertices);
			normals = std::move(app.normals);
			faces.clear();
			_atoms.clear();
			for (Face<T>&& f : app.faces){
				faces.push_back(f.migrate_to(&vertices, &normals));
			}
			for (AtomType<T>&& a : app._atoms){
				_atoms.push_back(a.migrate_to(&faces));
			}
			return *this;
		}

		//kezdo atomra mutato iterator
		Iterator begin() { return _atoms.begin(); }
		//utolso utani atomra mutato iterator
		Iterator end() { return _atoms.end(); }
		//kezdo atomra mutato konstans iterator
		ConstIterator begin() const { return _atoms.begin(); }
		//utolso utani atomra mutato konstans iterator
		ConstIterator end() const { return _atoms.end(); }

		//atomok elerese index alapjan
		const AtomType& atoms(size_t ind) const {return _atoms[ind];}

		//pontokat tartalmazo vektor
		const std::vector<Vector3<T>>& vertex_container() const { return vertices; }
		//normalvektorokat tartalmazo vektor
		const std::vector<Vector3<T>>& normal_container() const { return normals; }
		//lapokat tartalmazo vektor
		const std::vector<Face<T>>& face_container() const { return faces; }
		//konstans eleres a celtestre
		const TargetBody<T>& target_body() const {return *target;}
		//az atomszam lekerese
		int size() const { return _atoms.size(); }

		class CutResult{

			Approximation *a;
		public:
			CutResult(Approximation *a) : a(a){}

			//pozitiv oldali atom
			const AtomType* positive(){
				return static_cast<AtomType*>(a->cut_res.positive.get());
			}

			//negativ oldali atom
			const AtomType* negative(){
				return static_cast<AtomType*>(a->cut_res.negative.get());
			}

			//az eredeti atomot toroljuk beszurjuk az ujakat
			void choose_both(){
				if (a->pending()){
					a->choose_both();
				}
			}

			//a negativ atomot megtartjuk a pozitivat es az eredetit eldobjuk
			void choose_negative(){
				if (a->pending()){
					a->choose_negative();
				}
			}

			//a pozitiv atomot megtartjuk a negativot es az eredetit eldobjuk
			void choose_positive(){
				if (a->pending()){
					a->choose_positive();
				}
			}

			//a vagast visszavonjuk, az eredeti atom megmarad, a beszurt lapok es pontok torlodnek
			void undo(){
				if (a->pending()){
					a->undo();
				}
			}
		};


		//az adott indexu atom elvagasa megadott sikkal
		//az eredmenykent keletkezett atomok, lapok es pontok bekerulnek a taroloba
		CutResult cut(size_t ind, const Plane<T>& p){
			if (pending()) undo();
			last_cut = (int)ind;
			cut_res = _atoms[ind].cut_by(p);
			return CutResult(this);
		}

		//az adott iterator altal mutatott atom elvagasa megadott sikkal
		CutResult cut(Iterator pos, const Plane<T>& p){
			return cut(pos - _atoms.begin(), p);
		}

		//pontosan akkor igaz ha az utolso vagast meg nem fejeztuk be visszavonassal vagy elfogado valasztassal
		bool pending() const {
			return last_cut != -1;
		}

		//TODO: az approximacios dolgokat meg a szomszedsagi viszonyokat meg kell irni
		//a nemkello lapok, pontok es normalisok kitorlese a tartolokbool 
		void garbage_collection(){
			if (pending()) return;
			std::vector<int> needed(faces.size(),0);
			//kello lapok kiszurese
			for (const AtomType& a : _atoms){
				for (int ind : a.indicies()) needed[ind] = 1; //az adott lap kell
			}
			std::vector<Face<T>> new_faces;
			std::vector<Connection> new_connections;
			int deleted = 0;
			for (int i = 0; i < faces.size();++i){
				if (needed[i]) { //az adott lap kell
					needed[i] = deleted; //elotte ennyit toroltunk, annyival kell lejjebb vinni
					new_faces.push_back(std::move(faces[i])); //elrakjuk az uj laptartoba
					new_connections.push_back(connections[i]);
				}
				else{
					++deleted;
				}
			}
			connections = std::move(new_connections);
			for (int i = 0; i < connections.size(); ++i) {
				if (connections[i].other_face >= 0)
					connections[i].other_face -= needed[connections[i].other_face];
			}
			faces = std::move(new_faces); //a kello lapok
			for (AtomType& a : _atoms){
				for (int& ind : a.indicies()){
					ind -= needed[ind]; //a megmarado lapok indexeit el kell tolni ha toroltunk elotte
				}
			}
			//kello pontok es normalisok kiszurese
			needed.clear();
			needed.resize(vertices.size(), 0);
			std::vector<int> normal_needed(normals.size(),0);
			for (const AtomType& a : _atoms)
				for (const Face<T>& f : a){
					normal_needed[f.normal_index()] = 1;
					for (int ind : f.indicies())
						needed[ind] = 1;
				}
					
			std::vector<Vector3<T>> new_vertices, new_normals;
			deleted = 0;
			for (int i = 0; i < vertices.size();++i){
				if (needed[i]){
					needed[i] = deleted;
					new_vertices.push_back(vertices[i]);
				}
				else{
					++deleted;
				}
			}
			deleted = 0;
			for (int i = 0; i < normals.size(); ++i){
				if (normal_needed[i]){
					normal_needed[i] = deleted;
					new_normals.push_back(normals[i]);
				}
				else{
					++deleted;
				}
			}
			vertices = std::move(new_vertices);
			normals = std::move(new_normals);
			for (AtomType& a : _atoms)
				for (Face<T>& f : a){
					f.normal_index() -= normal_needed[f.normal_index()];
					for (int& ind : f.indicies())
						ind -= needed[ind];
				}

		}

		CutResult last_cut_result() {
			return CutResult(this);
		}

		//szamitott test
		//TODO: a belso lapos bohockodast meg kell csinalni
		Body<T> approximated_body(InsideHandling mode = InsideHandling::LeaveOut) {
			garbage_collection();
			std::vector<int> ind{};
			for (int i = 0; i < connections.size(); ++i) {
				if (connections[i].other_atom == -1) {
					ind.push_back(i);
				}
			}
			return Body<T>(&faces, ind);
		}

	};

}

#endif