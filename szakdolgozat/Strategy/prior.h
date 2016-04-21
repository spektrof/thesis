#pragma once

/*	Keszitette: Lukacs Peter

	A strategiahoz tartozo atomrendezest ez az osztaly vegzi el. Innen kerdezzuk le az aktualis sorrendet.
*/

#include "..\Engine\approximator.h"

template <typename T, template<typename> class V>
class PriorityQue
{
	V<T>* PriorityFunctions = new V<T>();
	typedef float (V<T>::*GETTER)(const T*,const int&) const;

	GETTER m_getterFunc;

	/*Egy atomot az id-ja es a hozza tartozo tulajdonsag ertekkel hatarozok meg*/
	struct Data {
		int id;
		float value;
		Data(const int& _id,const float& vol = 0) : id(_id), value(vol) {}
	};

	std::vector<Data> order;

public:
	PriorityQue(GETTER getterFunc)
	{
		m_getterFunc = getterFunc;
	}
	~PriorityQue() {}

	/*Beszuro rendezes*/
	void insert(const int& _id, const T* atom)
	{
		float tmp_val = (PriorityFunctions->*m_getterFunc)(atom,_id);
		std::vector<Data>::iterator it = order.begin();

		for (; it != order.end() && it->value > tmp_val; ++it) { }

		order.insert(it, Data(_id, tmp_val) );
	}

	void erase(const int& e)
	{
		order.erase(order.begin() + e);
	}

	void clear()
	{
		order.clear();
	}

	void SetComparer(GETTER getterfunc)
	{
		m_getterFunc = getterfunc;
	}

	/*TODO: priorresult & Data viszonya*/
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

	/*Legregebben hasznalt atomok id-jait tartalmazo vektor frissitese*/
	void SetLastUse(std::vector<int>* lu)
	{
		PriorityFunctions->SetLastUse(lu);
		
		for (std::vector<Data>::iterator it = order.begin(); it != order.end(); ++it)
		{
			it->value = (PriorityFunctions->*m_getterFunc)(NULL, it->id);
		}

	//	std::sort(order.begin(), order.end(), [&, this](const Data& lhs, const Data& rhs) { return lhs.value > rhs.value; });
	}

};