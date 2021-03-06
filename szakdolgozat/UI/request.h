#pragma once

/*	Keszitette: Lukacs Peter

	A felhasznaloi felulet es a approximalast vegzo program kozotti kommunikaciohoz szukseges keres.
*/

#include "../Engine/approximator.h"

/*Esemeny tipusa*/
enum ToDo{
	NONE,
	ACCEPT,
	CUTTING,
	UNDO,
	NEWPLANE,
	NEWSTRATEGY,
	NEWCUTTINGMODE,
	NEWDISPLAY,
	ACTYPECHANGED,
	RESTART,
	NEXTATOM,
	PREVATOM,
	RECALCULATING,
	MORESTEPS,
	EXPORT,
	IMPORT,
};

/*Atomvalasztasi opcio*/
enum ChoiceMode {
	VOLUME,
	DIAMETER,
	UNTOUCHED,
	OPTIMALPARAMETER,
	OPTIMALDIAMETER,
	OPTIMALVOLUME,
};

/*Vagosik valasztasi opcio*/
enum CuttingMode {
	MANUAL,
	MATCHEDEACHPOINT,
	DIAMETERCENTROID,
	RANDOMNORMALCENTROID,
	MATCHEDRANDOMSURFACE,
	RANDOMUNDERFACE,
};

/*Elfogadas tipusa*/
enum TypeOfAccept
{
	NEGATIVE,
	POSITIVE,
	BOTH,
	INVALID,
};

/*Megjelenitendo atomok tipusa*/
enum Display
{
	LIVE,
	RELEVANT,
	ONLYACTIVE,
};

/*3D Koordinatak*/
struct Coord {
	float x, y, z;
	Coord(float a, float b, float c) : x(a), y(b), z(c) {}
	template<typename T>
	operator approx::Vector3<T>() const { return approx::Vector3<T>(x, y, z);  }
};

struct Request {
	ToDo eventtype;
	ChoiceMode choice;
	CuttingMode cut_mode;
	TypeOfAccept type;
	Display disp;

	Coord plane_coord;
	Coord plane_norm;

	ushort CountsOfCutting;
	std::string filename;

	Request(const ToDo t = ToDo::NONE, const ChoiceMode ch = ChoiceMode(0), const CuttingMode cu = CuttingMode(0),
		    const TypeOfAccept a = TypeOfAccept(0), const Display d = Display(0), const Coord p = Coord(0,0,0), const Coord n = Coord(1,0,0), const ushort c = 1, std::string f = "")
		: eventtype(t), choice(ch), cut_mode(cu), type(a), disp(d), plane_coord(p), plane_norm(n), CountsOfCutting(c), filename(f) { }
};