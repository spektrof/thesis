#pragma once

#include "Utils/Utility.h"

template <typename T, template<typename> class V>
class PriorityQue
{
	V<T>* PriorityFunctions;
	typedef float (V<T>::*GETTER)(T) const;

	GETTER m_getterFunc;

	struct Data {
		int id;
		T AtomData;
		float value;
		Data(const int& _id, const T& atom,const float& vol = 0) : id(_id), AtomData(atom),value(vol) {}
	};

	std::vector<Data> order;

public:
	PriorityQue(GETTER getterFunc)
	{
		m_getterFunc = getterFunc;
	}

	//TODO: beszuro rendezes -> Accept eset�n kihagyhat� a sorter megh�v�s
	void insert(const int& _id, const T& atom)
	{
		order.push_back( Data(_id, atom, (PriorityFunctions->*m_getterFunc)(atom)) );
	}

	void clear()
	{
		order.clear();
	}

	void sorter()
	{
		/*At lehet meg gondolni hogy legyen, azt gondolom hogy check szempontj�b�l kell csak ez a volume
		  De ha akarjuk ellenorizni akkor kell ez az iter�ci� - viszont �gy insertben �rtelmetlenn� v�lik a sz�mol�sa -> t�bbsz�r lefut
		*/
		for (std::vector<Data>::iterator it = order.begin(); it != order.end(); ++it)
		{
			it->value = (PriorityFunctions->*m_getterFunc)(it->AtomData);
		}

		//std::sort(order.begin(), order.end(), [&, this](const Data& lhs, const Data& rhs) { return (lhs.AtomData.*m_getterFunc)() > (rhs.AtomData.*m_getterFunc)(); });
		std::sort(order.begin(), order.end(), [&, this](const Data& lhs, const Data& rhs) { return lhs.value > rhs.value; });
	}

	void SetComparer(GETTER getterfunc)
	{
		m_getterFunc = getterfunc;
	}

	std::vector<Utility::ResultData> GetOrder() const
	{
		std::vector<Utility::ResultData> result;
		result.clear();

		for (std::vector<Data>::const_iterator it = order.begin(); it != order.end(); ++it)
		{
			result.push_back(Utility::ResultData(it->id,it->value));
		}

		return result;
	}

};