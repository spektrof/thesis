#ifndef VECTOR3_H_INCLUDED
#define VECTOR3_H_INCLUDED


//
// Keszitette Toth Mate
// 2 es 3 dimenzios vektorokat reprezentalo sablonok az alapmuveleteikkel egyutt, valamint float es double pontossagra peldanyositott tipusok.
// A tipusok kepesek dolgozni tetszoleges skalar tipussal melyre értelmezve vannak a +,-,*,/,+=,-=,*=,/=,==,!= muveletek.
// A vektorok eseten lehetseges lenne a szarmaztatas hasznalata, am az kulonbozo dimenziok eltero celja miatt az elkulonitest valasztottam.
//

#include <cmath>

namespace approx{

	//Ketto dimenzios vektor, felkeszitve matematikai muveletek ellatasara
	template <class T> struct Vector2{
		typedef T ScalarType;

		T x, y; //koordinatak

		//konstruktorok
		Vector2(T _x=0, T _y=0) : x(_x), y(_y){}
		Vector2& operator =(const Vector2&) = default;

		//helyben muveletvegzo operatorok
		Vector2& operator +=(const Vector2& other){
			x += other.x;
			y += other.y;
			return *this;
		}
		Vector2& operator -=(const Vector2& other){
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vector2& operator *=(T num){
			x *= num;
			y *= num;
			return *this;
		}
		Vector2& operator /=(T num){
			x /= num;
			y /= num;
			return *this;
		}

		//vektorok kozotti osszeadas es kivonas
		Vector2 operator + (const Vector2& other) const{ return Vector2(x + other.x, y + other.y); }
		Vector2 operator - (const Vector2& other) const{ return Vector2(x - other.x, y - other.y); }
		
		//skalarral szorzas es osztas
		Vector2 operator * (T num) const{ return Vector2(x*num, y*num); }
		Vector2 operator / (T num) const{ return Vector2(x/num, y/num); }
		
		Vector2 operator -() const {
			return Vector2(x*-1, y*-1);
		}

		//hossz szamitas 2-es norma szerint
		T length() const { return sqrt(x*x+y*y); }
		
		//ugyanabba az iranyba mutato egysegvektor
		Vector2 normalized() const { T len = length(); return Vector2(x / len, y / len); }

		//egysegvektorra alakitas
		void normalize(){ *this /= length(); }

		//egyenloseg teszteles
		bool operator == (const Vector2& v)const { return x == v.x && y == v.y; }
		bool operator != (const Vector2& v) const { return x != v.x || y != v.y; }

	};

	//Harom dimenzios vektor felkeszitve matematikai muveletekre
	template <class T> struct Vector3{
		typedef T ScalarType;
		T x, y, z; //koordinatak

		//konstruktorok
		Vector3(T _x=0, T _y=0, T _z=0) :x(_x), y(_y), z(_z){}
		Vector3(const Vector3& a) :x(a.x), y(a.y), z(a.z){}

		//ertekadas
		Vector3& operator = (const Vector3&) = default;
		
		//helyben vegzett muveletek
		Vector3& operator +=(const Vector3& other){
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		Vector3& operator -=(const Vector3& other){
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}
		Vector3& operator *=(T number){
			x *= number;
			y *= number;
			z *= number;
			return *this;
		}
		Vector3& operator /=(T number){
			x /= number;
			y /= number;
			z /= number;
			return *this;
		}

		//matematikai muveletek
		Vector3 operator + (const Vector3& other) const {
			return Vector3(x+other.x, y+other.y,z+other.z);
		}
		Vector3 operator - (const Vector3& other) const {
			return Vector3(x - other.x, y - other.y, z - other.z);
		}
		Vector3 operator * (T num) const {
			return Vector3(x*num, y*num, z*num);
		}
		Vector3 operator / (T num) const {
			return Vector3(x/num, y/num, z/num);
		}

		Vector3 operator -() const {
			return Vector3(x*-1, y*-1, z*-1);
		}

		//2-es norma szerinti hossz
		T length() const { return sqrt(x*x+y*y+z*z); }

		//egysegvektorra alakitas
		void normalize(){
			*this /= length();
		}

		//ugyanabba az iranyba mutato egysegvektor
		Vector3 normalized() const {
			T len = length();
			return Vector3(x / len, y / len, z / len);
		}

		//egyenloseg teszteles
		bool operator == (const Vector3& v)const { return x == v.x && y == v.y && z == v.z; }
		bool operator != (const Vector3& v) const { return x != v.x || y != v.y || z!=v.z; }
	};


	//muveletek a skalarral valo szorzas megcserelhetosegere
	template <class T> Vector2<T> operator* (T num, const Vector2<T>& vec){
		return vec*num;
	}

	template <class T> Vector3<T> operator* (T num, const Vector3<T>& vec){
		return vec*num;
	}
	
	//skalarszorzasok
	template <class T> T dot(const Vector3<T>& a,const Vector3<T>& b){
		return a.x*b.x + a.y*b.y + a.z*b.z;
	}

	template <class T> T dot(const Vector2<T>& a, const Vector2<T>& b){
		return a.x*b.x + a.y*b.y;
	}

	//keresztszorzat
	template <class T> Vector3<T> cross(const Vector3<T>& u, const Vector3<T>& v){
		return Vector3<T>(u.y*v.z-u.z*v.y,
			              u.z*v.x-u.x*v.z,
						  u.x*v.y-u.y*v.x);
	}

	//pontosan akkor igaz, ha a megadott vektorok ccw sorrendben vannak felsorolva
	template <class T> bool ccw(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c){
		Vector2<T> u = c - b, v = a - b;
		return u.x*v.y - u.y*v.x > 0;
	}

	//kiszamolja a v1 es v2 altal bezart szog sinusat
	template <class T> T sin(const Vector3<T>& v1, const Vector3<T>& v2) {
		return cross(v1, v2).length() / (v1.length() * v2.length());
	}

	//kiszamolja az a-b es c-b altal bezart szog sinusat
	template <class T> T sin(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& c) {
		return sin(a - b, c - b);
	}

	//kiszamolja a v1 es v2 altal bezart szog cosinusat
	template <class T> T cos(const Vector3<T>& v1, const Vector3<T>& v2) {
		return dot(v1, v2) / (v1.length() *v2.length());
	}

	//kiszamolja az a-b es c-b altal bezart szog cosinusat
	template <class T> T cos(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& c) {
		return cos(a - b, c - b);
	}

	//konkret float es double skalarok feletti vektor tipusok
	typedef Vector2<float> Vector2f;
	typedef Vector3<float> Vector3f;
	typedef Vector2<double> Vector2d;
	typedef Vector3<double> Vector3d;

}

#endif