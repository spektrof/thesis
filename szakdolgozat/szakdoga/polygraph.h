#ifndef POLYGRAPH_H_INCLUDED
#define POLYGRAPH_H_INCLUDED


//
// Keszitette: Toth Mate
// Az itt talalhato muveletek ketto dimenzios pontokbol allo grafokat alakitanak at sokszogekke.
//

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "poly2.h"


namespace approx{

	template <class T> struct Less2d{ //rendezesi muvelet a map tipussal valo hasznalathoz
		bool operator ()(const Vector2<T>& a, const Vector2<T>& b) const{
			return a.x < b.x || (a.x == b.x && a.y < b.y);
		}
	};

	template <class T> using Graph = std::map < Vector2<T>, std::vector<Vector2<T>>, Less2d<T> > ;

	//template <class T> void depth_first_search(const Graph<T>& graph, std::set<Vector2<T>,Less2d<T>>& visited, std::vector<Vector2<T>>& path, std::vector<Polygon2<T>>& result){
	//	visited.insert(path.back());
	//	for (const Vector2<T>& pt : graph.at(path.back())){
	//		if (!visited.count(pt)){//arra a pontra meg nem mentem
	//			path.push_back(pt);
	//			visited.insert(pt);
	//			depth_first_search(graph, visited, path, result);
	//			path.pop_back();
	//		}
	//		else{
	//			std::vector<Vector2<T>>::iterator it = find(path.begin(), path.end()-1,pt);
	//			//korre talaltam
	//			if (it != path.end()-1 && it != path.end()-2){ 
	//				Polygon2<T> tmp(it, path.end());
	//				//a kesobbi alkalmazasok miatt fix ccw sorrendbe rakom
	//				if (!tmp.is_ccw()) std::reverse(tmp.begin(), tmp.end());
	//				result.emplace_back(std::move(tmp));
	//			}
	//		}
	//	}
	//}

	template <class T> void depth_first_search(const Graph<T>& graph, std::set<Vector2<T>, Less2d<T>>& visited, std::vector<Vector2<T>>& path, std::vector<Polygon2<T>>& result) {
		visited.insert(path.back());
		std::vector<Vector2<T>> list = graph.at(path.back());
		std::sort(list.begin(), list.end(), [=](const Vector2<T>& a, const Vector2<T>& b) {
			Vector2<T> v1 = (a - path.back()).normalized(), v2 = (b - path.back()).normalized();
			return atan2(v1.y, v1.x) < atan2(v2.y, v2.x);
		});
		for (const Vector2<T>& pt : list) {
			if (!visited.count(pt)) {//arra a pontra meg nem mentem
				path.push_back(pt);
				visited.insert(pt);
				depth_first_search(graph, visited, path, result);
				path.pop_back();
			}
			else {
				std::vector<Vector2<T>>::iterator it = find(path.begin(), path.end() - 1, pt);
				//korre talaltam
				if (it != path.end() - 1 && it != path.end() - 2) {
					Polygon2<T> tmp(it, path.end());
					//a kesobbi alkalmazasok miatt fix ccw sorrendbe rakom
					if (!tmp.is_ccw()) std::reverse(tmp.begin(), tmp.end());
					result.emplace_back(std::move(tmp));
				}
			}
		}
	}



	template <class T> std::vector<Polygon2<T>> get_polys(const Graph<T>& graph){
		std::vector<Polygon2<T>> result;
		std::set<Vector2<T>,Less2d<T>> visited;
		std::vector<Vector2<T>> path;
		for (typename Graph<T>::const_iterator it = graph.begin(); it != graph.end(); ++it){
			if (!visited.count(it->first)){
				path.push_back(it->first);
				depth_first_search(graph, visited, path, result);
				path.pop_back();
			}
		}
		return result;
	}


}

#endif