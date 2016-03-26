#pragma once
#include "../szakdoga/approximator.h"

enum ToDo{
	NONE,
	ACCEPT,
	CUTTING,
	UNDO,
	NEWPLANE,
	NEWSTRATEGY,
	NEWCUTTINGMODE,
	RESTART,
	NEXTATOM,
	PREVATOM,
	RECALCULATING,
	MORESTEPS,
};

//RANDOM
enum ChoiceMode {
	VOLUME,
	DIAMETER,
	UNTOUCHED,
	OPTIMALPARAMETER,
	OPTIMALATMERO,
	OPTIMALVOLUME,
};

//	Vágó sík :

enum CuttingMode {
	MANUAL,
	OPTIMALLAPALATT,
	MINDENPONTRAILLESZTETT,
	OPTIMFELULETILL,
	GLOBHIBAOPTIM,
	ATMEROREMEROLEGESSULYP,
	RANDOM,
	RANDOMNORMALCENTROID,
	RANDOMFELULETILLESZT,
	RANDOMLAPALATT,
};


enum TypeOfAccept
{
	NEGATIVE,
	POSITIVE,
	BOTH,
};

struct Coord {
	float x, y, z;
	Coord(float a, float b, float c) : x(a), y(b), z(c) {}
	operator approx::Vector3<float>() const { return approx::Vector3<float>(x, y, z);  }
};


struct Request {
	ToDo eventtype;
	ChoiceMode choice;
	CuttingMode cut_mode;
	TypeOfAccept type;
	/*Vagosik*/
	Coord plane_coord;
	Coord plane_norm;
	/*Tobb vagas*/
	ushort CountsOfCutting;

	Request(const ToDo t = ToDo::NONE, const ChoiceMode ch = ChoiceMode(0), const CuttingMode cu = CuttingMode(0),
		    const TypeOfAccept a = TypeOfAccept(0), const Coord p = Coord(0,0,26), const Coord n = Coord(0,0,1), const ushort c = 1)
		: eventtype(t), choice(ch), cut_mode(cu), type(a), plane_coord(p), plane_norm(n), CountsOfCutting(c) { }
};