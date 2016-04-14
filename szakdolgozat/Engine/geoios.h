#ifndef GEOIO_H_INCLUDED
#define GEOIO_H_INCLUDED

//
// Keszitette: Toth Mate
// A fajl celja az iostream-ekkel valo altalanos kommunikacio, foleg debuggolasi celbol.
//

#include <iostream>
#include "vectors.h"
#include "face.h"
#include "body.h"
#include "poly2.h"

template <class T> std::ostream& operator << (std::ostream& o, const approx::Vector2<T>& v){
	o << v.x << ',' << v.y;
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const approx::Vector3<T>& v){
	o << v.x << ',' << v.y << ',' << v.z;
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const std::vector<approx::Vector3<T>>& v){
	o << "[ ";
	for (const auto& e : v){
		o << "(" << e << ") ";
	}
	o << "]";
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const approx::Face<T>& f){
	o << "Face---------------------------- \n";
	for (auto vert : f){
		o << vert << '\n';
	}
	o << "----------------------------/Face\n";
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const approx::Body<T>& b) {
	for (auto& f : b) {
		o << f;
	}
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const approx::Plane<T>& p){
	o << "Plane( { " << p.normal() << " }, " << p.signed_distance() << ")";
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const approx::Line<T>& p){
	o << "Line( { " << p.normal() << " }, " << p.signed_distance() << ")";
	return o;
}

template <class T> std::ostream& operator << (std::ostream& o, const approx::Polygon2<T>& p){
	o << "Polygon--------------------------\n";
	for (auto v : p){
		o << v << "\n";
	}
	o << "-------------------------/Polygon\n";
	return o;
}

template <class T> void print_indicies(std::ostream& o, const approx::Face<T>& f){
	o << '('; 
	for (int i = 0; i < (int)f.size()-1; ++i){
		o << f.indicies(i) << ", ";
	}
	if (f.size()>0){
		o << f.indicies(f.size() - 1);
	}
	o << ")\n";
}

#endif