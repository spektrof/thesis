#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

using  ns = std::chrono::nanoseconds;
using get_time = std::chrono::steady_clock ;

/*Copyzva lett ez a namespace resz*/
namespace Utility{
	struct PriorResult {
		int id;
		float value;
		PriorResult(const int& _id, const float& vol) : id(_id),value(vol) {}
	};
};

#include "../Strategy/prior.h"



class Sikidom
{
public:
	Sikidom(const float& _a) : a(_a) { }
	virtual ~Sikidom() {}
	virtual float GetArea() const { return -1; }
	virtual float GetPerimeter() const { return -1; }
	virtual std::string Sides() const { std::stringstream txt; return txt.str();}

protected:
	 float a;
	 
};

class Negyzet : public Sikidom
{
public:
	Negyzet(const float& _a) : Sikidom(_a) { }
	~Negyzet() {}
	
	float GetArea() const { return a*a; }
	float GetPerimeter() const { return 4*a;}
	std::string Sides() const { std::stringstream txt; txt <<"Negyzet " << a; return txt.str(); }
	
protected:
	 
};

class Teglalap : public Sikidom
{
public:
	Teglalap(const float& _a,const float& _b) : Sikidom(_a),b(_b) {}
	~Teglalap() {}
	
	float GetArea() const { return a*b; }
	float GetPerimeter()const { return 2*(a+b);}
	std::string Sides() const { std::stringstream txt; txt <<"Teglalap " << a << " " << b; return txt.str(); }
	
protected:
	 float b;
};

class SzabHaromszog : public Sikidom
{
public:
	SzabHaromszog(const float& _a) : Sikidom(_a) {}
	~SzabHaromszog() {}
	
	float GetArea() const { return a*a*sqrt(3)/4.0f; }
	float GetPerimeter()const { return 3*a;}
	std::string Sides() const { std::stringstream txt; txt <<"SzabHaromszog " << a; return txt.str(); }
	
protected:
	 float b;
};


template <class T>
class SikidomRendezok
{
	std::vector<int>* lastuse;
	
public:
	SikidomRendezok(std::vector<int>* l = NULL) : lastuse(l) { }
	~SikidomRendezok() {}
	
	float Area(const T* atom, const int& id) const
	{
			return atom->GetArea();
	}
	float Perimeter(const T* atom, const int& id) const
	{
			return atom->GetPerimeter();
	}
};

int main()
{
	std::vector<Sikidom*> members;
	members.resize(5);
	members[0] = new Negyzet(3);
	members[1] = new Teglalap(5,2);
	members[2] = new Negyzet(2);
	members[3] = new Teglalap(0.5,10);
	members[4] = new Negyzet(2);
	
	PriorityQue<Sikidom,SikidomRendezok> prior = PriorityQue<Sikidom,SikidomRendezok>(&SikidomRendezok<Sikidom>::Area);
	//--------------------------------------------------
	std::cout<<"Egyszeru feltoltes(0-3):\n Minden esetet lefed (Ures sorba szur, Elejere, vegere, kozepere szur)\n";
	prior.insert(0,members[0]);
	prior.insert(1,members[1]);
	prior.insert(2,members[2]);
	prior.insert(3,members[3]);
	
	std::vector<Utility::PriorResult> result = prior.GetOrder();
	std::vector<int> indexes = prior.GetPriorIndexes();
	
	std::cout<<"\tGetOrder:\n";
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	
	std::cout<<"\tGetPriorIndexes:\n";
	std::for_each(indexes.begin(), indexes.end(), [](const int& a) { std::cout << "\t\t"<< a << "\n"; });
	std::cout << "\n";
	//--------------------------------------------------
	std::cout<<"Ures sor eseten: (clear)\n";
	prior.clear();
	
	result = prior.GetOrder();
	indexes = prior.GetPriorIndexes();
	
		std::cout<<"\tGetOrder:\n";
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	
	std::cout<<"\tGetPriorIndexes:\n";
	std::for_each(indexes.begin(), indexes.end(), [](const int& a) { std::cout << "\t\t" << a << "\n"; });
	std::cout << "\n";
	//--------------------------------------------------
	std::cout<<"Masik rendezo fv. valasztas a dokumentacioban leirt modon: \n";
	prior.SetComparer(&SikidomRendezok<Sikidom>::Perimeter);
	prior.clear();
	prior.insert(0,members[0]);
	prior.insert(1,members[1]);
	prior.insert(2,members[2]);
	prior.insert(3,members[3]);
	prior.insert(4,members[4]);
	
	result = prior.GetOrder();
	indexes = prior.GetPriorIndexes();
	
	std::cout<<"\tGetOrder:\n";
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	
	std::cout<<"\tGetPriorIndexes:\n";
	std::for_each(indexes.begin(), indexes.end(), [](const int& a) { std::cout << "\t\t" << a << "\n"; });
	std::cout << "\n";
	//--------------------------------------------------
	std::cout<<"NULL atom beszurasa\n";
	prior.insert(11,NULL);
	
	result = prior.GetOrder();
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	//-------------------------------------------------
	std::cout<<"Egy elem torlese:\n";
	std::cout<<"\tLetezo eleme (2.):\n";
	prior.erase(2);
	
	result = prior.GetOrder();
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";

	std::cout<<"\tNem letezo eleme (10.):\n";
	prior.erase(10);
	
	result = prior.GetOrder();
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	
	std::cout<<"\tNem letezo eleme hataron(4.):\n";
	prior.erase(4);
	
	result = prior.GetOrder();
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	
	std::cout<<"\tLetezo eleme hataron(3.):\n";
	prior.erase(3);
	
	result = prior.GetOrder();
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	
	std::cout<<"\tLetezo eleme hataron(0.):\n";
	prior.erase(0);
	
	result = prior.GetOrder();
	std::for_each(result.begin(), result.end(), [](const Utility::PriorResult& a) { std::cout << "\t\t" << a.id << " " << a.value << "\n"; });
	std::cout << "\n";
	//-------------------------------------------------
	std::cout<<"Volumen teszt: feloltes + csere a doksiban leirt modon\n";
	std::cout<<"Futas kozbeni gyorsasagot maximalizalnank! ??????????????????? valtas nelkul\n";
	const int NUMBEROFMEMBERS = 1000;
	std::random_device rd;
	
	prior.clear();
	members.clear();
	members.resize(NUMBEROFMEMBERS);
	for (int i=0;i<NUMBEROFMEMBERS;++i)
	{
		switch(rd()%3)
		{
			case 0:
				members[i] = new Negyzet(rd()%965);
				break;
			case 1:{
				members[i] = new Teglalap(rd()%1000,rd()%1000);
				break;
			case 2:
				members[i] = new SzabHaromszog(rd()%1300);
				break;}
		}
	}
	auto start = get_time::now();
	std::cout<<"PriorityQue:\n";
	for (int i=0;i<NUMBEROFMEMBERS;++i)
	{
		prior.insert(i,members[i]);
	}
	std::cout<<"Elso 10 tag:\n";
	result = prior.GetOrder();
	for (int i=0;i<10;++i)
	{
		std::cout<< "\t" << result[i].id << " " << result[i].value<< " " << members[result[i].id]->Sides()  <<"\n";
	}
	std::cout<<"\n";
	
    auto diff1 = get_time::now() - start;
	std::cout<<"Elapsed time is :  "<< std::chrono::duration_cast<ns>(diff1).count()<<" ns\n";
	
	std::cout<<"Masik rendezo fv. valasztas: (teruletre)\n";
	prior.SetComparer(&SikidomRendezok<Sikidom>::Area);
	prior.clear();
	for (int i=0;i<NUMBEROFMEMBERS;++i)
	{
		prior.insert(i,members[i]);
	}
	
	std::cout<<"Elso 10 tag:\n";
	result = prior.GetOrder();
	for (int i=0;i<10;++i)
	{
		std::cout<< "\t" << result[i].id << " " << result[i].value<< " " << members[result[i].id]->Sides() <<"\n";
	}
	std::cout<<"\n";
	
	diff1 = get_time::now() - start;
	std::cout<<"Elapsed time is :  "<< std::chrono::duration_cast<ns>(diff1).count()<<" ns\n";
	
	//-------------------------------------------------------------------------
	std::cout<<"Vector + std::sort (nlogn) - nem a priorityque val\nFELTOLTES (egyesevel ahogy valojaban tortenne)\n";
	start = get_time::now();
	
	std::vector<Utility::PriorResult> vtest_qsort;
	for (int i=0;i<NUMBEROFMEMBERS;++i)
	{
		vtest_qsort.push_back(Utility::PriorResult(i,members[i]->GetPerimeter()));
		std::sort(vtest_qsort.begin(),vtest_qsort.end(),[](const Utility::PriorResult& a,const Utility::PriorResult& b){ return a.value > b.value; });
	}
	for (int i=0;i<10;++i)
	{
		std::cout<< "\t" << vtest_qsort[i].id << " " << vtest_qsort[i].value<< " " << members[vtest_qsort[i].id]->Sides() <<"\n";
	}
	
	auto diff2 = get_time::now() - start;
	std::cout<<"Elapsed time is :  "<< std::chrono::duration_cast<ns>(diff2).count()<<" ns\n";
	
	std::cout<<"VALTAS:\n";
	for (int i=0;i<NUMBEROFMEMBERS;++i)
	{
		vtest_qsort[i].value = members[vtest_qsort[i].id]->GetArea();
	}
	std::sort(vtest_qsort.begin(),vtest_qsort.end(),[](const Utility::PriorResult& a,const Utility::PriorResult& b){ return a.value > b.value; });
	for (int i=0;i<10;++i)
	{
		std::cout<< "\t" << vtest_qsort[i].id << " " << vtest_qsort[i].value<< " " << members[vtest_qsort[i].id]->Sides() <<"\n";
	}
	
	 diff2 = get_time::now() - start;
	std::cout<<"Elapsed time is :  "<< std::chrono::duration_cast<ns>(diff2).count()<<" ns\n";
	
	/*if (diff1<diff2) std::cout<<"PRIORITYQUE GYORSABB\n";
	else if (diff1>diff2) std::cout<<"SIMAVECTOR + std::sort GYORSABB\n";
	else std::cout<<"EGYENLO\n";*/
	return 0;
}