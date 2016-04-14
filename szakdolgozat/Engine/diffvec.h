#ifndef DIFFVEC_H_INCLUDED
#define DIFFVEC_H_INCLUDED
namespace approx {
	template <class T> struct DifferentVector3 { //rendezesi muvelet a map tipussal valo hasznalathoz
		bool operator ()(const Vector3<T>& a, const Vector3<T>& b) const {
			return	a.x < b.x ||
				(a.x == b.x && a.y < b.y) ||
				(a.x == b.x && a.y == b.y && a.z < b.z);
		}
	};
}
#endif