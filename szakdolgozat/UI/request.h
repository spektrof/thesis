#pragma once

enum ToDo{
	NONE,
	ACCEPT,
	CUTTING,
	UNDO,
	RESTART,
	NEWPLANE,
	NEWSTRATEGY,
};

//RANDOM
enum AutomaticControl {
	VOLUME,
	ATMERO,
	ERINTETLEN,
	OPTIMALPARAMETER,
	OPTIMALATMERO,
	OPTIMALVOLUME,
	RANDOMMANUAL,
};

//	Vágó sík :

enum UserControl {
	MANUAL,
	RANDOMNORMALthCENTROID,
	ATMEROREMEROLEGESSULYP,
	RANDOMLAPUNDER,
	OPTIMALLAPUNDER,
	ALLPOINTILLESZTETT,
	RANDOMFELULETILLESZT,
	OPTIMFELULETILL,
	GLOBHIBAOPTIM,
};


enum TypeAccept
{
	NEGATIVE,
	POSITIVE,
	BOTH,
};

struct Coord {
	float x, y, z;
	Coord(float a, float b, float c) : x(a),y(b),z(c) {}
};


struct Request {
	ToDo happen;
	UserControl uc;
	AutomaticControl ac;
	TypeAccept ta;
	Coord plane_coord;
	Coord plane_norm;
	bool IsUserControl;

	Request(ToDo t = ToDo::NONE, UserControl u = UserControl(0),AutomaticControl a = AutomaticControl(0), Coord c = Coord(0,0,0), Coord n = Coord(1,0,0), bool ius = false) 
		: happen(t), uc(u), ac(a), plane_coord(c),plane_norm(n), IsUserControl(ius) { }
};