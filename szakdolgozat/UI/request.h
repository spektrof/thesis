#pragma once

enum ToDo{
	NONE,
	ACCEPT,
	CUTTING,
	UNDO,
	RESTART,
	NEWPLANE,
};

enum UserControl {
	AUTOMATIC,
	RANDOMPLANE, 
	LYINGONTRIANGLE, 
	LEASTCOPLANAR,
	BESTFITTING			
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
	TypeAccept ta;
	Coord plane_coord;
	Coord plane_norm;

	Request(ToDo t = ToDo::NONE, UserControl u = UserControl::AUTOMATIC, Coord c = Coord(0,0,0), Coord n = Coord(1,0,0) ) 
		: happen(t), uc(u), plane_coord(c),plane_norm(n) { }
};