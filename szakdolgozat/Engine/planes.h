#ifndef PLANE_H_INCLUDED
#define PLANE_H_INCLUDED

//
// Keszitette Toth Mate
// 2 dimenzios vonalak es 3 dimenzios sikok reprezentelesara kepes sablonok.
// A sikok feladata az adatszerkezetekben a ter ketteosztasa, valamint a pontok osztalyozasa.
// Mivel a 2 es 3 dimenzios sikok kozel ugyanazokat a feladatokat latjak el, kozos sablonon, a hipersikon alapulnak.
// A 3 dimenzios sik tobb muveletet is ellat a programban ezert kulon alosztalyt kapott.
//

#include <utility>
#include "vectors.h"

namespace approx{

	//Hipersik tetszoleges dimenziohoz 
	template <class Vector> class HyperPlane{
		typedef typename Vector::ScalarType ScalarType;
		Vector n; //normalvektor
		ScalarType dist; //elojeles tavolsag az origotol a normalvektor iranyaban
	public:
		//sik egy normalissal es egy ponttal megadva
		HyperPlane(const Vector& _normal, const Vector& _point) : n(_normal.normalized()),dist(dot(n,_point)){}
		//sik egy normalissal es elojeles tavolsaggal megadva
		HyperPlane(const Vector& _normal, ScalarType distance) : n(_normal.normalized()), dist(distance){}
		//masolo konstruktor
		HyperPlane(const HyperPlane&) = default;
		HyperPlane(){}
		HyperPlane& operator = (const HyperPlane&) = default;

		//elojeles tavolsag az origotol a normalvektor iranyaban
		ScalarType signed_distance() const { return dist; }
		
		//normalvektor lekerdezes
		//Vector& normal() { return n; }
		Vector normal() const{ return n; }

		//egy pont amely a sikon helyezkedik el es a normalvektor skalarszorosa
		Vector example_point() const{
			return n*dist;
		}

		//a pont elojeles tavolsaga a siktol, pozitiv ha a normalvektor iranyaban a sikon tul esik es negativ kulonben
		ScalarType classify_point(const Vector& p) const {
			return dot(p, n) - dist;
		}

		//a siktol valo tavolsaga a pontnak
		ScalarType distance(const Vector& p) const {
			return abs(classify_point(p));
		}

		bool valid() const { return n.length() > 0; }
	};

	template <class T> using Line = HyperPlane < Vector2<T> > ;

	//template <class T> using Plane = HyperPlane < Vector3<T> >;
	template <class T> class Plane : public HyperPlane < Vector3<T> > {
	public:
		Plane(const Vector3<T>& n, const Vector3<T>& p) : HyperPlane<Vector3<T>>(n, p){}
		Plane(const Vector3<T>& n, T d) : HyperPlane<Vector3<T>>(n, d){}
		Plane() = delete;
		
		using HyperPlane < Vector3<T> >::normal;
		using HyperPlane < Vector3<T> >::example_point;
		
		//kiszamol ket meroleges vektort amelyek felhasznalhatoak tengelykent
		//a sikon fekvo alakzatok ket dimenzioba kepezesenel, ugyelve a numerikus elonyossegre
		std::pair<Vector3<T>, Vector3<T>> ortho2d() const {
			T s1 = sqrt(normal().x*normal().x + normal().y*normal().y),
			  s2 = sqrt(normal().x*normal().x + normal().z*normal().z),
			  s3 = sqrt(normal().y*normal().y + normal().z*normal().z);
			if (s1 >= s2 && s1 >= s3){
				Vector3<T> w(-(normal().y / s1), normal().x / s1, 0);
				return{ w.normalized(), cross(normal(), w).normalized() };
			}
			else if (s2 >= s3 && s2 >= s1){
				Vector3<T> w(-(normal().z / s2), 0, normal().x / s2);
				return{ w.normalized(), cross(normal(), w).normalized() };
			}
			else{
				Vector3<T> w(0, -(normal().z / s3), normal().y / s3);
				return{ w.normalized(), cross(normal(), w).normalized() };
			}			
		}

		//a masik sikkal elmetszem ezt a sikot, a metszet egyenest
		//ennek a siknak a koordinatarendszereben adom meg
		Line<T> intersection_line(const Plane& p) const {
			if (abs(dot(normal(), p.normal())) > 0.9999f) return Line<T>();
			std::pair<Vector3<T>, Vector3<T>> base = ortho2d();
			Vector3<T> up = cross(normal(), p.normal()).normalized();
			Vector3<T> w = cross(up, normal()).normalized();
			Vector3<T> tmppt = example_point();
			T pdist = p.classify_point(tmppt);
			T cosine = -dot(p.normal(), w);
			Vector3<T> pt1 = tmppt + w*(pdist / cosine);
			Vector2<T> line_normal(dot(p.normal(), base.first), dot(p.normal(), base.second)),
					   line_point(dot(pt1, base.first), dot(pt1, base.second));
			return Line<T>(line_normal, line_point);
		}

	};

}

#endif