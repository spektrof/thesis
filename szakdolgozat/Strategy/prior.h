#pragma once

/*	Keszitette: Lukacs Peter

	A strategiahoz tartozo atomrendezest ez az osztaly vegzi el. Innen kerdezzuk le az aktualis sorrendet.
*/

#include "..\Engine\approximator.h"

template <typename T, template<typename> class V>
class PriorityQue
{
	V<T>* PriorityFunctions;
	typedef float (V<T>::*GETTER)(const T*,const int&) const;	//a tulajdonsagot meghatarozo fuggveny cime

	GETTER m_getterFunc;

	/*Egy atomot az id-ja es a hozza tartozo tulajdonsag ertekkel hatarozok meg*/
	struct Data {
		int id;
		float value;
		Data(const int& _id,const float& vol = 0) : id(_id), value(vol) {}
	};

	std::vector<Data> order;

public:
	PriorityQue(GETTER getterFunc, std::vector<int>* lu)
	{
		PriorityFunctions = new V<T>(lu);
		m_getterFunc = getterFunc;
	}
	~PriorityQue() { delete PriorityFunctions;  }

	/*Beszuro rendezes*/
	void insert(const int& _id, const T* atom)
	{
		float tmp_val = (PriorityFunctions->*m_getterFunc)(atom,_id);
		std::vector<Data>::iterator it = order.begin();

		for (; it != order.end() && it->value > tmp_val; ++it) { }

		order.insert(it, Data(_id, tmp_val) );
	}

	/*Torli a parameterben megadott indexu tagot a sorbol*/
	void erase(const int& e)
	{
		order.erase(order.begin() + e);
	}

	/*Torli a sort*/
	void clear()
	{
		order.clear();
	}

	/*Tulajdonsag meghatarozo fuggveny csereje*/
	void SetComparer(GETTER getterfunc)
	{
		m_getterFunc = getterfunc;
	}

	/*Visszaadja az atomok sorrendjet (id,ertek) formaban*/
	std::vector<Utility::PriorResult> GetOrder() const
	{
		std::vector<Utility::PriorResult> result;
		result.clear();

		for (std::vector<Data>::const_iterator it = order.begin(); it != order.end(); ++it)
		{
			result.push_back(Utility::PriorResult(it->id,it->value));
		}

		return result;
	}

	/*Visszaadja a sor indexeit*/
	std::vector<int> GetPriorIndexes() const
	{
		std::vector<int> result;
		result.clear();

		for (std::vector<Data>::const_iterator it = order.begin(); it != order.end(); ++it)
		{
			result.push_back(it->id);
		}

		return result;
	}

	/*Legregebben hasznaltsagot leiro vektor cimenek a frissitese*/
	void SetLastUse(std::vector<int>* lu)
	{
		PriorityFunctions->SetLastUse(lu);
	}

	/*A legregebben hasznalt vektor szerinti frissites (menet kozbeni)*/
	void RefreshLastUseValues()
	{
		for (std::vector<Data>::iterator it = order.begin(); it != order.end(); ++it)
		{
			it->value = (PriorityFunctions->*m_getterFunc)(NULL, it->id);
		}
	}

};