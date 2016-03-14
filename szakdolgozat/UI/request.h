#pragma once

enum ToDo{
	NONE,
	ACCEPT,
	CUTTING,
	UNDO,
	RESTART,
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
	int x, y, z;
	Coord(int a,int b, int c) : x(a),y(b),z(c) {}
};



struct Request {
	ToDo happen;
	UserControl uc;
	TypeAccept ta;
	Coord _3Dcoord;
	//vezérlo stratégiákhoz még ami kell - minden 1 structban - a 2 enum alapján el lehet dönteni mi lesz értelmes benne s mi nem

	Request(ToDo t = ToDo::NONE, UserControl u = UserControl::AUTOMATIC, Coord c = Coord(0,0,0) ) : happen(t), uc(u), _3Dcoord(c) { }
};