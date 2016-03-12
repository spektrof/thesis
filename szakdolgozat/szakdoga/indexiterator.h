#ifndef INDEXITERATOR_H_INCLUDED
#define INDEXITERATOR_H_INCLUDED

//
// Keszitette: Toth Mate
// Absztrakt iterator sablon tipusok melyek modosithato es konstans hozzaferest nyujtanak indexelt adatokhoz
// Tobb adatszerkezetnek van szuksege olyan adatok kezelesere melyekre indexsorozattal hivatkoznak, ez az iterator tipus ezt segiti.
// Ezek a tipusok kielegitik a C++ szabvany RandomAccessIterator elvarasait, hasznalhatoak az std konyvtar algoritmusaival
//

#include <vector>
#include <iterator>

namespace approx{

	template <class T> class ConstIndexIterator : std::iterator < std::random_access_iterator_tag, const T, int > {
		const std::vector<T>* vecs;
		const std::vector<int>* inds;
		size_t pos;
	public:
		ConstIndexIterator(const std::vector<T>* v, const std::vector<int>* i, size_t p) : vecs(v), inds(i), pos(p){}
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::value_type value_type;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::difference_type difference_type;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::reference reference;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::pointer pointer;
		ConstIndexIterator(){}
		ConstIndexIterator(const ConstIndexIterator&) = default;
		ConstIndexIterator& operator = (const ConstIndexIterator&) = default;
		ConstIndexIterator operator ++() {
			++pos;
			return *this;
		}
		ConstIndexIterator operator ++(int){
			IndexIterator t = *this;
			++pos;
			return t;
		}
		ConstIndexIterator operator --() {
			--pos;
			return *this;
		}
		ConstIndexIterator operator --(int){
			IndexIterator t = *this;
			--pos;
			return t;
		}
		ConstIndexIterator operator +=(difference_type cnt){
			pos += cnt;
			return *this;
		}
		ConstIndexIterator operator -=(difference_type cnt){
			pos -= cnt;
			return *this;
		}
		ConstIndexIterator operator +(difference_type cnt) const {
			return IndexIterator(vecs, inds, pos + cnt);
		}
		ConstIndexIterator operator -(difference_type cnt) const {
			return IndexIterator(vecs, inds, pos - cnt);
		}
		difference_type operator -(const ConstIndexIterator& other) const {
			return pos - other.pos;
		}
		bool operator !=(const ConstIndexIterator& other) const {
			return other.pos != pos || vecs != other.vecs || inds != other.inds;
		}
		const T& operator * () const {
			return vecs->operator[](inds->operator[](pos));
		}
		const T* operator -> () const {
			return &(vecs->operator[](inds->operator[](pos)));
		}
		const T& operator [] (difference_type ind) const {
			return vecs->operator[](inds->operator[](pos + ind));
		}
	};

	template <class T> class IndexIterator : std::iterator < std::random_access_iterator_tag, T, int > {
		std::vector<T>* vecs;
		const std::vector<int>* inds;
		size_t pos;
	public:
		IndexIterator(std::vector<T>* v, const std::vector<int>* i, size_t p) : vecs(v), inds(i), pos(p){}
		typedef std::random_access_iterator_tag iterator_category;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::value_type value_type;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::difference_type difference_type;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::reference reference;
		typedef typename std::iterator<std::random_access_iterator_tag, const Vector3<T>, int>::pointer pointer;
		IndexIterator(){}
		IndexIterator(const IndexIterator&) = default;
		IndexIterator& operator = (const IndexIterator&) = default;
		IndexIterator operator ++() {
			++pos;
			return *this;
		}
		IndexIterator operator ++(int){
			IndexIterator t = *this;
			++pos;
			return t;
		}
		IndexIterator operator --() {
			--pos;
			return *this;
		}
		IndexIterator operator --(int){
			IndexIterator t = *this;
			--pos;
			return t;
		}
		IndexIterator operator +=(difference_type cnt){
			pos += cnt;
			return *this;
		}
		IndexIterator operator -=(difference_type cnt){
			pos -= cnt;
			return *this;
		}
		IndexIterator operator +(difference_type cnt) const {
			return IndexIterator(vecs, inds, pos + cnt);
		}
		IndexIterator operator -(difference_type cnt) const {
			return IndexIterator(vecs, inds, pos - cnt);
		}
		difference_type operator -(const IndexIterator& other) const {
			return pos - other.pos;
		}
		bool operator !=(const IndexIterator& other) const {
			return other.pos != pos || vecs != other.vecs || inds != other.inds;
		}
		T& operator * (){
			return vecs->operator[](inds->operator[](pos));
		}
		T* operator -> (){
			return &(vecs->operator[](inds->operator[](pos)));
		}
		T& operator [] (difference_type ind){
			return vecs->operator[](inds->operator[](pos + ind));
		}
	};


}

#endif