#pragma once

#include "Utils/Utility.h"
#include "szakdoga\approximator.h"

template <typename T, template<typename> class V>
class PriorityQue
{
	V<T>* PriorityFunctions = new V<T>();
	typedef float (V<T>::*GETTER)(const T*,const int& _id) const;

	GETTER m_getterFunc;

	struct Data {
		int id;
		//const T* AtomData;
		float value;
		Data(const int& _id/*, const T* atom*/,const float& vol = 0) : id(_id), /*AtomData(atom),*/value(vol) {}
	};

	std::vector<Data> order;

public:
	PriorityQue(GETTER getterFunc)
	{
		m_getterFunc = getterFunc;
	}

	/*Beszuro rendezes*/
	void insert(const int& _id, const T* atom)
	{
		float tmp_val = (PriorityFunctions->*m_getterFunc)(atom,_id);
		std::vector<Data>::iterator it = order.begin();

		for (; it != order.end() && it->value > tmp_val; ++it) { }

		order.insert(it, Data(_id/*, atom*/, tmp_val) );
	}

	void erase(const int& e)
	{
		order.erase(order.begin() + e);
	}

	void clear()
	{
		order.clear();
	}

	/*Only use when we choose an other strategy*/
/*	void sorter()
	{
		for (std::vector<Data>::iterator it = order.begin(); it != order.end(); ++it)
		{
			it->value = (PriorityFunctions->*m_getterFunc)(it->AtomData,it->id);
		}

		//std::sort(order.begin(), order.end(), [&, this](const Data& lhs, const Data& rhs) { return (lhs.AtomData.*m_getterFunc)() > (rhs.AtomData.*m_getterFunc)(); });
		std::sort(order.begin(), order.end(), [&, this](const Data& lhs, const Data& rhs) { return lhs.value > rhs.value; });
	}*/

	void SetComparer(GETTER getterfunc)
	{
		m_getterFunc = getterfunc;
	}

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

	void SetLastUse(std::vector<int>* lu)
	{
		PriorityFunctions->SetLastUse(lu);
		/*Azert van szukseg erre mert beszuro rendezessel dolgozunk -> csak egyszer szamoljuk az erteket viszont ez mindig valtozik
			DE nem k�ne mindig lefuttatni mert csak a last_use hoz kell
		*/
		// itt nem sz�m�t az atom csak az index
		for (std::vector<Data>::iterator it = order.begin(); it != order.end(); ++it)
		{
			it->value = (PriorityFunctions->*m_getterFunc)(NULL, it->id);
		}

		std::sort(order.begin(), order.end(), [&, this](const Data& lhs, const Data& rhs) { return lhs.value > rhs.value; });
	}
};