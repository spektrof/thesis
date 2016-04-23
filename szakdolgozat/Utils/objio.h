#ifndef OBJIO_H_INCLUDED
#define OBJIO_H_INCLUDED

//
// Keszitette: Toth Mate
// Az ObjectLoader osztaly statikus fuggvenyei segitsegevel objektumok betoltesere kepes.
// Maga az osztaly nem tobb mint egy pszeudo nevter, azonban a hatasos betoltes erdekeben
// a friend minosites segitsegevel kepesek hozzaferni a kello privat adattagokhoz, metodusait
// pedig tetszolegesen valtoztathatjuk anelkul hogy a tobbi fajlhoz kene nyulni.
// Ez elhetoseget ad arra, hogy kesobb tobb formatumot is tamogasson.
//

#include <sstream>
#include <fstream>
#include <algorithm>
#include "objrepair.h"
#include "targetbody.h"
#include <map>
#include "geoios.h"

namespace approx {

	//statikus osztaly betoltesi funkciokhoz
	template <class T> class ObjectLoader {

		//hiba tortent a betoltes soran ezert minden szemetet kitorlunk ami esetleg bekerult es hamissal terunk vissza
		static bool exit_cleanup(TargetBody<T>& tb) {
			tb.vecs.clear();
			tb.normals.clear();
			tb.faces.clear();
			tb.bdy = Body<T>(nullptr, {});
			return false;
		}

		static bool parse_node(const std::string& str, int& vind, int& nind) {
			std::stringstream ss(str);
			if (!(ss >> vind)) return false;
			char c;
			ss >> c;
			if (ss.eof()) {
				nind = -1;
				return true;
			}
			if (ss.peek() != '/') {
				int tmp;
				ss >> tmp;
				if (ss.eof()) {
					nind = -1;
					return true;
				}
			}
			ss >> c;
			return (bool)(ss >> nind);
		}


		//vaz metodus a .obj fajlformatum feldolgozasahoz, template parameterei a "javito" tipusok
		//a javito tipusok dolga, hogy sajat belatasuk szerint osszevonjanak 2 numerikusan egyezonek nyilvanitott vektort
		//a javitas celja hogy a fajlokbol bekerult testeken vegzett muveletek a numerikus pontossagra minel kevesbe legyenek erzekenyek
		template <class Repair1, class Repair2> static bool load_obj_temp(const std::string& filename, TargetBody<T>& tb, Repair1& tmp_vecs, Repair2& tmp_normals, bool triangulate) {
			tb.vecs.clear();
			tb.normals.clear();
			tb.faces.clear();
			std::ifstream f(filename);
			if (!f) return exit_cleanup(tb);
			std::vector<Vector3<T>> accum_normals;
			std::string line; //sor a fajlban
			while (std::getline(f, line)) {
				std::string::size_type ind = line.find('#');
				if (ind != std::string::npos) {
					line.erase(ind);
				}
				if (line.size()) {
					std::stringstream stream(line);//a sor darabolasara hasznalt stream
					std::string beg;//sorkezdo szimbolum mely azonositja a sor tartalmat
					T x, y, z; //skalar adatok beolvasasara
					stream >> beg;
					if (beg == "v") {//vertexpont
						stream >> x >> y >> z;
						if (stream.fail()) return exit_cleanup(tb);
						tmp_vecs.push_back({ x, y, z });
					}
					else if (beg == "vn") {//normalvektor
										   //nem ezeket a normalokat fogom hasznalni, de cw ccw ellenorzeshez kellenek
						stream >> x >> y >> z;
						if (stream.fail()) return exit_cleanup(tb);
						accum_normals.push_back({ x, y, z });
					}
					else if (beg == "f") {//lap indexekkel leirva
						std::vector<int> inds;
						Vector3<T> sum_normal, calculated_normal;
						std::string node;
						int vind, nind;
						while (stream >> node && node.length()) {
							if (!parse_node(node, vind, nind)) return exit_cleanup(tb);
							inds.push_back(vind - 1);
							if (nind > 0) {
								sum_normal += accum_normals[nind - 1];
							}
						}
						if ((stream.fail() && !stream.eof()) || inds.size()<3) return exit_cleanup(tb);
						//keresek egy egyenesszogtol tavolabbi belso szoget es annal nezek egy keresztszorzatot hogy jo iranyba alljon a lap
						int k = 2;
						while (k < (int)inds.size() &&
							std::max(sin(tmp_vecs[inds[0]], tmp_vecs[inds[1]], tmp_vecs[inds[k]]),
								sin(tmp_vecs[inds[k - 2]], tmp_vecs[inds[k - 1]], tmp_vecs[inds[k]])) < 0.01f) ++k;
						if (k == (int)inds.size()) k = 2;
						if (sin(tmp_vecs[inds[0]], tmp_vecs[inds[1]], tmp_vecs[inds[k]]) > sin(tmp_vecs[inds[k - 2]], tmp_vecs[inds[k - 1]], tmp_vecs[inds[k]])) {
							calculated_normal = cross(tmp_vecs[inds[k]] - tmp_vecs[inds[1]], tmp_vecs[inds[0]] - tmp_vecs[inds[1]]).normalized();
						}
						else {
							calculated_normal = cross(tmp_vecs[inds[k]] - tmp_vecs[inds[k - 1]], tmp_vecs[inds[k - 2]] - tmp_vecs[inds[k - 1]]).normalized();
						}

						if (dot(calculated_normal, sum_normal) < 0) {
							calculated_normal *= -1;
							std::reverse(inds.begin(), inds.end());
						}

						inds = tmp_vecs.transform_range(inds);
						tmp_normals.push_back(calculated_normal);
						if (triangulate) {
							for (int i = 2; i < (int)inds.size(); ++i) {
								tb.faces.emplace_back(&tb.vecs,
									std::vector<int>{inds[0], inds[i - 1], inds[i]},
									&tb.normals,
									tmp_normals.transform_index(tmp_normals.size() - 1));
							}
						}
						else {
							tb.faces.emplace_back(&tb.vecs, std::move(inds), &tb.normals, tmp_normals.transform_index(tmp_normals.size() - 1));
						}
					}
				}
			}
			if (!tb.faces.size()) return exit_cleanup(tb); //ha nincsenek lapjaink azt is ervenytelen esetnek konyveljuk el
			tb.vecs = tmp_vecs.needed_vecs(); //elteszem az osszerendezett pontokat
			tb.normals = tmp_normals.needed_vecs();
			tb.bdy = Body<T>(&tb.faces, std::move(TargetBody<T>::range(tb.faces.size())));
			return true; //minden megfelelo
		}


	public:
		// .obj formatumu fajl betoltese
		// Eredmenye pontosan akkor igaz, ha sikerult betolteni egy ervenyes fajlt a megadott nevvel.
		// A normalvektorokat a lapok pontjaibol szamolom, elkerulve a szetallo normalisok okozta hibat.
		// Amennyiben a fajl tartalmaz normalisokat, azokat felhasznalom a cw-ccw sorrenduseg ellenorzesere,
		// ha nincs normalvektor adat a fajlban, felteszem, hogy ccw sorrendben megadott lapjaim vannak.
		// Szinten felteszem, hogy a fajl helyesen van megadva, nem vegzek rajta korrekciot.
		static bool load_obj(const std::string& filename, TargetBody<T>& tb, bool triangulate = true) {
			NullRepair<T> d1, d2;
			return load_obj_temp(filename, tb, d1, d2, triangulate);
		}

		// .obj formatumu fajl betoltese
		// Eredmenye pontosan akkor igaz, ha sikerult betolteni egy ervenyes fajlt a megadott nevvel.
		// A normalvektorokat a lapok pontjaibol szamolom, elkerulve a szetallo normalisok okozta hibat.
		// Amennyiben a fajl tartalmaz normalisokat, azokat felhasznalom a cw-ccw sorrenduseg ellenorzesere,
		// ha nincs normalvektor adat a fajlban, felteszem, hogy ccw sorrendben megadott lapjaim vannak.
		// A megadott epszilonon beluli euklideszi tavolsagu pontokat egyenloknek veszem es explicit osszevonom.
		static bool load_obj(const std::string& filename, TargetBody<T>& tb, T epsilon, bool triangulate = true) {
			if (epsilon <= 0) return load_obj(filename, tb, triangulate);
			RepairVector<T> d1(epsilon);
			NullRepair<T> d2;
			return load_obj_temp(filename, tb, d1, d2, triangulate);
		}
	};

	//onjektumok fajlba irasara alkalmas osztaly mely statikus metodusaival kepes fajlba irni a modelleket
	template <class T> class ObjectWriter {

		//a megadott vektorlistat "beginning x y z" sorok formajaban a megadott streambe irja
		static void write_obj_vector(std::ostream& os, const std::string& beginning, const std::vector<Vector3<T>>& v) {
			for (const Vector3<T>& e : v) {
				os << beginning << " " << e.x << " " << e.y << " " << e.z << std::endl;
			}
			os << std::endl;
		}
		//f v//vn ... formatumban lap streambe iratas
		static void write_obj_face(std::ostream& os, const Face<T>& face) {
			int n = face.normal_index();
			os << "f ";
			if (face.is_ccw()) {
				for (int ind : face.indicies()) {
					os << ind + 1 << "//" << n + 1 << " ";
				}
			}
			else {
				for (auto it = face.indicies().rbegin(); it != face.indicies().rend(); ++it) {
					os << (*it + 1) << "//" << n + 1 << " ";
				}
			}
			os << std::endl;
		}

	public:
		//az egy tarolon osztozo, iteratorok altal megadott Body vagy abból leszármazó, testek kiiratasa egyetlen fajlba
		//a parameterben atadott iteratorok altal mutatott tipusnak a Body<T> publikus interfacevel kell rendelkezniuk
		template <class BodyIter> static void save_obj(const std::string& filename, BodyIter first, BodyIter last) {
			std::ofstream f(filename);
			while (first != last && !first->size()) ++first; //elmegyek az elso testig amiben van is lap 
			if (first != last) { //ha van nem ures test a listaban kiiratom a vertex es normal vektorokat
				f << "#No. vertices: " << first->faces(0).vertex_container()->size() << ":" << std::endl;
				write_obj_vector(f, "v", *(first->faces(0).vertex_container()));
				f << "#No. normals " << first->faces(0).normal_container()->size() << ":" << std::endl;
				write_obj_vector(f, "vn", *(first->faces(0).normal_container()));
			}
			int id = 0;
			while (first != last) { //vegigiteralok a testeken es csoportokba szervezve kiiratom oket
				f << "o atom" << id++ << std::endl;
				for (const Face<T>& fac : *first) {
					write_obj_face(f, fac);
				}
				f << std::endl;
				++first;
			}
		}

		//az egesz approximacio kiiratasa egyetlen, .obj formatumu fajlba
		static void save_obj(const std::string& filename, const Approximation<T>& app) {
			save_obj(filename, app.begin(), app.end());
		}

		//egyetlen test kiiratasa a fajlba, a kello szelektalas mellett mellett
		static void save_obj(const std::string& filename, const Body<T>& b) {
			std::ofstream f(filename);
			std::map<int, int> normals, verts;
			std::vector<Vector3<T>> w_normals, w_verts;
			for (const Face<T>& face : b) {
				if (!normals.count(face.normal_index())) {
					w_normals.push_back(face.normal());
					normals[face.normal_index()] = w_normals.size();
				}
				for (int i : face.indicies()) {
					if (!verts.count(i)) {
						w_verts.push_back(face.vertex_container()->operator[](i));
						verts[i] = w_verts.size();
					}
				}
			}
			f << "#No. vertices: " << w_verts.size() << ":" << std::endl;
			write_obj_vector(f, "v", w_verts);
			f << "#No. normals: " << w_normals.size() << ":" << std::endl;
			write_obj_vector(f, "vn", w_normals);
			f << "o approx_body" << std::endl;
			for (const Face<T>& fac : b) {
				f << "f ";
				if (fac.is_ccw()) { //ccwben van
					for (int ind : fac.indicies()) {
						f << verts[ind] << "//" << normals[fac.normal_index()] << " ";
					}
				}
				else { //ccwbe forgatom
					for (auto it = fac.indicies().rbegin(); it != fac.indicies().rend(); ++it) {
						f << verts[*it] << "//" << normals[fac.normal_index()] << " ";
					}
				}
				f << std::endl;
			}
		}

	};
}

#endif